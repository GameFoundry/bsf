#include "BsCamera.h"
#include "BsCameraRTTI.h"
#include "BsMath.h"
#include "BsMatrix3.h"
#include "BsVector2.h"
#include "BsAABox.h"
#include "BsSphere.h"
#include "BsHardwareBufferManager.h"
#include "BsVertexBuffer.h"
#include "BsIndexBuffer.h"
#include "BsException.h"
#include "BsRenderSystem.h"
#include "BsSceneObject.h"

namespace BansheeEngine 
{
	const float Camera::INFINITE_FAR_PLANE_ADJUST = 0.00001f;

	Camera::Camera(const HSceneObject& parent, RenderTargetPtr target, float left, float top, float width, float height)
        : Component(parent), mProjType(PT_PERSPECTIVE), mHorzFOV(Radian(Math::PI/4.0f)), mFarDist(100000.0f), 
		mNearDist(100.0f), mAspect(1.33333333333333f), mOrthoHeight(1000), mRecalcFrustum(true), mRecalcFrustumPlanes(true), 
		mCustomViewMatrix(false), mCustomProjMatrix(false), mFrustumExtentsManuallySet(false), mIgnoreSceneRenderables(false), 
		mPriority(0), mLayers(0xFFFFFFFFFFFFFFFF), mCoreDirtyFlags(0xFFFFFFFF)
    {
		setName("Camera");

		mViewMatrix = Matrix4::ZERO;
		mProjMatrixRS = Matrix4::ZERO;

        invalidateFrustum();

		target->synchronize();
		mViewport = bs_shared_ptr<Viewport, PoolAlloc>(target, left, top, width, height);
    }

    Camera::~Camera()
    {
    }

	void Camera::setHorzFOV(const Radian& fov)
	{
		mHorzFOV = fov;
		invalidateFrustum();
		markCoreDirty();
	}

	const Radian& Camera::getHorzFOV() const
	{
		return mHorzFOV;
	}

	void Camera::setFarClipDistance(float farPlane)
	{
		mFarDist = farPlane;
		invalidateFrustum();
		markCoreDirty();
	}

	float Camera::getFarClipDistance() const
	{
		return mFarDist;
	}

	void Camera::setNearClipDistance(float nearPlane)
	{
		if (nearPlane <= 0)
		{
			BS_EXCEPT(InvalidParametersException, "Near clip distance must be greater than zero.");
		}

		mNearDist = nearPlane;
		invalidateFrustum();
		markCoreDirty();
	}

	float Camera::getNearClipDistance() const
	{
		return mNearDist;
	}

	const Matrix4& Camera::getProjectionMatrix() const
	{
		updateFrustum();

		return mProjMatrix;
	}

	const Matrix4& Camera::getProjectionMatrixRS() const
	{
		updateFrustum();

		return mProjMatrixRS;
	}

	const Matrix4& Camera::getViewMatrix() const
	{
		updateView();

		return mViewMatrix;
	}

	const ConvexVolume& Camera::getFrustum() const
	{
		// Make any pending updates to the calculated frustum planes
		updateFrustumPlanes();

		return mFrustum;
	}

	ConvexVolume Camera::getWorldFrustum() const
	{
		const Vector<Plane>& frustumPlanes = getFrustum().getPlanes();
		Matrix4 worldMatrix = SO()->getWorldTfrm();

		Vector<Plane> worldPlanes(frustumPlanes.size());
		UINT32 i = 0;
		for (auto& plane : frustumPlanes)
		{
			worldPlanes[i] = worldMatrix.multiply3x4(plane);
			i++;
		}

		return ConvexVolume(worldPlanes);
	}

	void Camera::calcProjectionParameters(float& left, float& right, float& bottom, float& top) const
	{ 
		if (mCustomProjMatrix)
		{
			// Convert clipspace corners to camera space
			Matrix4 invProj = mProjMatrix.inverse();
			Vector3 topLeft(-0.5f, 0.5f, 0.0f);
			Vector3 bottomRight(0.5f, -0.5f, 0.0f);

			topLeft = invProj.multiply(topLeft);
			bottomRight = invProj.multiply(bottomRight);

			left = topLeft.x;
			top = topLeft.y;
			right = bottomRight.x;
			bottom = bottomRight.y;
		}
		else
		{
			if (mFrustumExtentsManuallySet)
			{
				left = mLeft;
				right = mRight;
				top = mTop;
				bottom = mBottom;
			}

			else if (mProjType == PT_PERSPECTIVE)
			{
				Radian thetaY (mHorzFOV * 0.5f);
				float tanThetaY = Math::tan(thetaY);
				float tanThetaX = tanThetaY * mAspect;

				float half_w = tanThetaX * mNearDist;
				float half_h = tanThetaY * mNearDist;

				left = -half_w;
				right = half_w;
				bottom = -half_h;
				top = half_h;

				mLeft = left;
				mRight = right;
				mTop = top;
				mBottom = bottom;
			}
			else
			{
				float half_w = getOrthoWindowWidth() * 0.5f;
				float half_h = getOrthoWindowHeight() * 0.5f;

				left = -half_w;
				right = half_w;
				bottom = -half_h;
				top = half_h;

				mLeft = left;
				mRight = right;
				mTop = top;
				mBottom = bottom;
			}
		}
	}

	void Camera::updateFrustum() const
	{
		if (isFrustumOutOfDate())
		{
			float left, right, bottom, top;

			calcProjectionParameters(left, right, bottom, top);

			if (!mCustomProjMatrix)
			{
				float inv_w = 1 / (right - left);
				float inv_h = 1 / (top - bottom);
				float inv_d = 1 / (mFarDist - mNearDist);

				if (mProjType == PT_PERSPECTIVE)
				{
					float A = 2 * mNearDist * inv_w;
					float B = 2 * mNearDist * inv_h;
					float C = (right + left) * inv_w;
					float D = (top + bottom) * inv_h;
					float q, qn;

					if (mFarDist == 0)
					{
						// Infinite far plane
						q = Camera::INFINITE_FAR_PLANE_ADJUST - 1;
						qn = mNearDist * (Camera::INFINITE_FAR_PLANE_ADJUST - 2);
					}
					else
					{
						q = - (mFarDist + mNearDist) * inv_d;
						qn = -2 * (mFarDist * mNearDist) * inv_d;
					}

					mProjMatrix = Matrix4::ZERO;
					mProjMatrix[0][0] = A;
					mProjMatrix[0][2] = C;
					mProjMatrix[1][1] = B;
					mProjMatrix[1][2] = D;
					mProjMatrix[2][2] = q;
					mProjMatrix[2][3] = qn;
					mProjMatrix[3][2] = -1;
				} 
				else if (mProjType == PT_ORTHOGRAPHIC)
				{
					float A = 2 * inv_w;
					float B = 2 * inv_h;
					float C = - (right + left) * inv_w;
					float D = - (top + bottom) * inv_h;
					float q, qn;

					if (mFarDist == 0)
					{
						// Can not do infinite far plane here, avoid divided zero only
						q = - Camera::INFINITE_FAR_PLANE_ADJUST / mNearDist;
						qn = - Camera::INFINITE_FAR_PLANE_ADJUST - 1;
					}
					else
					{
						q = - 2 * inv_d;
						qn = - (mFarDist + mNearDist)  * inv_d;
					}

					mProjMatrix = Matrix4::ZERO;
					mProjMatrix[0][0] = A;
					mProjMatrix[0][3] = C;
					mProjMatrix[1][1] = B;
					mProjMatrix[1][3] = D;
					mProjMatrix[2][2] = q;
					mProjMatrix[2][3] = qn;
					mProjMatrix[3][3] = 1;
				}
			}

			RenderSystem* renderSystem = BansheeEngine::RenderSystem::instancePtr();
			renderSystem->convertProjectionMatrix(mProjMatrix, mProjMatrixRS);

			// Calculate bounding box (local)
			// Box is from 0, down -Z, max dimensions as determined from far plane
			// If infinite view frustum just pick a far value
			float farDist = (mFarDist == 0) ? 100000 : mFarDist;

			// Near plane bounds
			Vector3 min(left, bottom, -farDist);
			Vector3 max(right, top, 0);

			if (mCustomProjMatrix)
			{
				// Some custom projection matrices can have unusual inverted settings
				// So make sure the AABB is the right way around to start with
				Vector3 tmp = min;
				min.floor(max);
				max.ceil(tmp);
			}

			if (mProjType == PT_PERSPECTIVE)
			{
				// Merge with far plane bounds
				float radio = farDist / mNearDist;
				min.floor(Vector3(left * radio, bottom * radio, -farDist));
				max.ceil(Vector3(right * radio, top * radio, 0));
			}

			mBoundingBox.setExtents(min, max);

			mRecalcFrustum = false;
			mRecalcFrustumPlanes = true;
		}
	}

	bool Camera::isFrustumOutOfDate() const
	{
		return mRecalcFrustum;
	}

	void Camera::updateView() const
	{
		if (!mCustomViewMatrix)
		{
			Matrix3 rot;
			const Quaternion& orientation = sceneObject()->getWorldRotation();
			const Vector3& position = sceneObject()->getWorldPosition();

			mViewMatrix.makeView(position, orientation);
		}
	}

	void Camera::updateFrustumPlanes() const
	{
		updateFrustum();

		if (mRecalcFrustumPlanes)
		{
			Vector<Plane> frustumPlanes(6);
			Matrix4 combo = mProjMatrix;

			frustumPlanes[FRUSTUM_PLANE_LEFT].normal.x = combo[3][0] + combo[0][0];
			frustumPlanes[FRUSTUM_PLANE_LEFT].normal.y = combo[3][1] + combo[0][1];
			frustumPlanes[FRUSTUM_PLANE_LEFT].normal.z = combo[3][2] + combo[0][2];
			frustumPlanes[FRUSTUM_PLANE_LEFT].d = combo[3][3] + combo[0][3];

			frustumPlanes[FRUSTUM_PLANE_RIGHT].normal.x = combo[3][0] - combo[0][0];
			frustumPlanes[FRUSTUM_PLANE_RIGHT].normal.y = combo[3][1] - combo[0][1];
			frustumPlanes[FRUSTUM_PLANE_RIGHT].normal.z = combo[3][2] - combo[0][2];
			frustumPlanes[FRUSTUM_PLANE_RIGHT].d = combo[3][3] - combo[0][3];

			frustumPlanes[FRUSTUM_PLANE_TOP].normal.x = combo[3][0] - combo[1][0];
			frustumPlanes[FRUSTUM_PLANE_TOP].normal.y = combo[3][1] - combo[1][1];
			frustumPlanes[FRUSTUM_PLANE_TOP].normal.z = combo[3][2] - combo[1][2];
			frustumPlanes[FRUSTUM_PLANE_TOP].d = combo[3][3] - combo[1][3];

			frustumPlanes[FRUSTUM_PLANE_BOTTOM].normal.x = combo[3][0] + combo[1][0];
			frustumPlanes[FRUSTUM_PLANE_BOTTOM].normal.y = combo[3][1] + combo[1][1];
			frustumPlanes[FRUSTUM_PLANE_BOTTOM].normal.z = combo[3][2] + combo[1][2];
			frustumPlanes[FRUSTUM_PLANE_BOTTOM].d = combo[3][3] + combo[1][3];

			frustumPlanes[FRUSTUM_PLANE_NEAR].normal.x = combo[3][0] + combo[2][0];
			frustumPlanes[FRUSTUM_PLANE_NEAR].normal.y = combo[3][1] + combo[2][1];
			frustumPlanes[FRUSTUM_PLANE_NEAR].normal.z = combo[3][2] + combo[2][2];
			frustumPlanes[FRUSTUM_PLANE_NEAR].d = combo[3][3] + combo[2][3];

			frustumPlanes[FRUSTUM_PLANE_FAR].normal.x = combo[3][0] - combo[2][0];
			frustumPlanes[FRUSTUM_PLANE_FAR].normal.y = combo[3][1] - combo[2][1];
			frustumPlanes[FRUSTUM_PLANE_FAR].normal.z = combo[3][2] - combo[2][2];
			frustumPlanes[FRUSTUM_PLANE_FAR].d = combo[3][3] - combo[2][3];

			for(UINT32 i = 0; i < 6; i++) 
			{
				float length = frustumPlanes[i].normal.normalize();
				frustumPlanes[i].d /= -length;
			}

			mFrustum = ConvexVolume(frustumPlanes);
			mRecalcFrustumPlanes = false;
		}
	}

	float Camera::getAspectRatio(void) const
	{
		return mAspect;
	}

	void Camera::setAspectRatio(float r)
	{
		mAspect = r;
		invalidateFrustum();
		markCoreDirty();
	}

	const AABox& Camera::getBoundingBox() const
	{
		updateFrustum();

		return mBoundingBox;
	}

	void Camera::setProjectionType(ProjectionType pt)
	{
		mProjType = pt;
		invalidateFrustum();
		markCoreDirty();
	}

	ProjectionType Camera::getProjectionType() const
	{
		return mProjType;
	}

	void Camera::setCustomViewMatrix(bool enable, const Matrix4& viewMatrix)
	{
		mCustomViewMatrix = enable;
		if (enable)
		{
			assert(viewMatrix.isAffine());
			mViewMatrix = viewMatrix;
		}

		markCoreDirty();
	}

	void Camera::setCustomProjectionMatrix(bool enable, const Matrix4& projMatrix)
	{
		mCustomProjMatrix = enable;

		if (enable)
			mProjMatrix = projMatrix;

		invalidateFrustum();
		markCoreDirty();
	}

	void Camera::setOrthoWindow(float w, float h)
	{
		mOrthoHeight = h;
		mAspect = w / h;

		invalidateFrustum();
		markCoreDirty();
	}

	void Camera::setOrthoWindowHeight(float h)
	{
		mOrthoHeight = h;

		invalidateFrustum();
		markCoreDirty();
	}

	void Camera::setOrthoWindowWidth(float w)
	{
		mOrthoHeight = w / mAspect;

		invalidateFrustum();
		markCoreDirty();
	}

	float Camera::getOrthoWindowHeight() const
	{
		return mOrthoHeight;
	}

	float Camera::getOrthoWindowWidth() const
	{
		return mOrthoHeight * mAspect;	
	}

	void Camera::setFrustumExtents(float left, float right, float top, float bottom)
	{
		mFrustumExtentsManuallySet = true;
		mLeft = left;
		mRight = right;
		mTop = top;
		mBottom = bottom;

		invalidateFrustum();
		markCoreDirty();
	}

	void Camera::resetFrustumExtents()
	{
		mFrustumExtentsManuallySet = false;

		invalidateFrustum();
		markCoreDirty();
	}

	void Camera::getFrustumExtents(float& outleft, float& outright, float& outtop, float& outbottom) const
	{
		updateFrustum();

		outleft = mLeft;
		outright = mRight;
		outtop = mTop;
		outbottom = mBottom;
	}

    void Camera::invalidateFrustum() const
    {
		mRecalcFrustum = true;
		mRecalcFrustumPlanes = true;
    }

	Vector2I Camera::worldToScreenPoint(const Vector3& worldPoint) const
	{
		Vector2 clipPoint = worldToClipPoint(worldPoint);
		return clipToScreenPoint(clipPoint);
	}

	Vector2 Camera::worldToClipPoint(const Vector3& worldPoint) const
	{
		Vector3 viewPoint = worldToViewPoint(worldPoint);
		return viewToClipPoint(viewPoint);
	}

	Vector3 Camera::worldToViewPoint(const Vector3& worldPoint) const
	{
		return getViewMatrix().multiply3x4(worldPoint);
	}

	Vector3 Camera::screenToWorldPoint(const Vector2I& screenPoint) const
	{
		Vector2 clipPoint = screenToClipPoint(screenPoint);
		return clipToWorldPoint(clipPoint);
	}

	Vector3 Camera::screenToViewPoint(const Vector2I& screenPoint) const
	{
		Vector2 clipPoint = screenToClipPoint(screenPoint);
		return clipToViewPoint(clipPoint);
	}

	Vector2 Camera::screenToClipPoint(const Vector2I& screenPoint) const
	{
		Vector2 clipPoint;
		clipPoint.x = (float)(((screenPoint.x - mViewport->getX()) / mViewport->getWidth()) * 2.0f - 1.0f);
		clipPoint.y = (float)(((screenPoint.y - mViewport->getY()) / mViewport->getHeight()) * 2.0f - 1.0f);

		return clipPoint;
	}

	Vector3 Camera::viewToWorldPoint(const Vector3& viewPoint) const
	{
		return getViewMatrix().inverseAffine().multiply3x4(viewPoint);
	}

	Vector2I Camera::viewToScreenPoint(const Vector3& viewPoint) const
	{
		Vector2 clipPoint = viewToClipPoint(viewPoint);
		return clipToScreenPoint(clipPoint);
	}

	Vector2 Camera::viewToClipPoint(const Vector3& viewPoint) const
	{
		Vector4 projPoint = getProjectionMatrix().multiply(Vector4(viewPoint.x, viewPoint.y, viewPoint.z, 1.0f));

		if (projPoint.w > 1e-7f)
		{
			float invW = 1.0f / projPoint.w;
			projPoint.x *= invW;
			projPoint.y *= invW;
		}
		else
		{
			projPoint.x = 0.0f;
			projPoint.y = 0.0f;
		}

		return Vector2(projPoint.x, projPoint.y);
	}

	Vector3 Camera::clipToWorldPoint(const Vector2& clipPoint) const
	{
		Vector3 viewPoint = clipToViewPoint(clipPoint);
		return viewToWorldPoint(viewPoint);
	}

	Vector3 Camera::clipToViewPoint(const Vector2& clipPoint) const
	{
		return unprojectPoint(Vector3(clipPoint.x, clipPoint.y, 0.5f));
	}

	Vector2I Camera::clipToScreenPoint(const Vector2& clipPoint) const
	{
		Vector2I screenPoint;
		screenPoint.x = Math::roundToInt(mViewport->getX() + (clipPoint.x + 1.0f) * mViewport->getWidth() * 0.5f);
		screenPoint.y = Math::roundToInt(mViewport->getY() + (clipPoint.y + 1.0f) * mViewport->getHeight() * 0.5f);

		return screenPoint;
	}

	Ray Camera::screenPointToRay(const Vector2I& screenPoint) const
	{
		Vector2 clipPoint = screenToClipPoint(screenPoint);

		Vector3 near = unprojectPoint(Vector3(clipPoint.x, clipPoint.y, mNearDist));
		Vector3 far = unprojectPoint(Vector3(clipPoint.x, clipPoint.y, mNearDist + 1.0f));

		return Ray(near, Vector3::normalize(far - near));
	}

	Vector3 Camera::unprojectPoint(const Vector3& point) const
	{
		Vector4 dir4(point.x, point.y, 0.95f, 1.0f); // 0.95f arbitrary
		dir4 = getProjectionMatrix().inverse().multiply(dir4);

		Vector3 dir;
		dir.x = dir4.x;
		dir.y = dir4.y;
		dir.z = dir4.z;

		if (dir4.w > 1e-7f)
		{
			float invW = 1.0f / dir4.w;
			dir.x *= invW;
			dir.y *= invW;
			dir.z *= invW;

			// Find a point along a ray from camera origin to point on near plane we just found, 
			// at point.z distance from the origin

			float distToPlane = dir.dot(-Vector3::UNIT_Z);
			if (distToPlane >= 0.0f)
			{
				if (mProjType == PT_PERSPECTIVE)
					dir *= point.z / distToPlane;
				else
					dir += Vector3::UNIT_Z * (distToPlane - point.z);
			}
		}
		else
		{
			dir.x = 0.0f;
			dir.y = 0.0f;
			dir.z = 0.0f;
		}

		return Vector3(dir.x, dir.y, dir.z);
	}

	CameraProxyPtr Camera::_createProxy() const
	{
		CameraProxyPtr proxy = bs_shared_ptr<CameraProxy>();
		proxy->layer = mLayers;
		proxy->priority = mPriority;
		proxy->projMatrix = getProjectionMatrix();
		proxy->viewMatrix = getViewMatrix();
		proxy->worldMatrix = SO()->getWorldTfrm();
		proxy->viewport = mViewport->clone();
		proxy->frustum = getFrustum();
		proxy->ignoreSceneRenderables = mIgnoreSceneRenderables;
		proxy->worldPosition = SO()->getWorldPosition();

		return proxy;
	}

	RTTITypeBase* Camera::getRTTIStatic()
	{
		return CameraRTTI::instance();
	}

	RTTITypeBase* Camera::getRTTI() const
	{
		return Camera::getRTTIStatic();
	}
} 

/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#include "CmCamera.h"
#include "CmCameraRTTI.h"

#include "CmMath.h"
#include "CmMatrix3.h"
#include "CmVector2.h"
#include "CmAxisAlignedBox.h"
#include "CmSphere.h"
#include "CmHardwareBufferManager.h"
#include "CmVertexBuffer.h"
#include "CmIndexBuffer.h"
#include "CmException.h"
#include "CmRenderSystem.h"
#include "CmGameObject.h"

namespace CamelotEngine {
	const float Camera::INFINITE_FAR_PLANE_ADJUST = 0.00001f;

    //-----------------------------------------------------------------------
	Camera::Camera(GameObjectPtr parent)
        : Component(parent),
		mProjType(PT_PERSPECTIVE), 
		mFOVy(Radian(Math::PI/4.0f)), 
		mFarDist(100000.0f), 
		mNearDist(100.0f), 
		mAspect(1.33333333333333f), 
		mOrthoHeight(1000),
		mFrustumOffset(Vector2::ZERO),
		mFocalLength(1.0f),
		mLastParentOrientation(Quaternion::IDENTITY),
		mLastParentPosition(Vector3::ZERO),
		mRecalcFrustum(true), 
		mRecalcFrustumPlanes(true),
		mRecalcWorldSpaceCorners(true),
		mRecalcVertexData(true),
		mCustomViewMatrix(false),
		mCustomProjMatrix(false),
		mFrustumExtentsManuallySet(false)
    {
		updateView();
		updateFrustum();

        // Reasonable defaults to camera params
        mFOVy = Radian(Math::PI/4.0f);
        mNearDist = 100.0f;
        mFarDist = 100000.0f;
        mAspect = 1.33333333333333f;
        mProjType = PT_PERSPECTIVE;

        invalidateFrustum();

        // Init matrices
        mViewMatrix = Matrix4::ZERO;
        mProjMatrixRS = Matrix4::ZERO;
    }

    //-----------------------------------------------------------------------
    Camera::~Camera()
    {
    }
	void Camera::init(RenderTargetPtr target, float left, float top, float width, float height, int ZOrder)
	{
		mViewport = ViewportPtr(new Viewport(target, left, top, width, height, ZOrder));
	}
	//-----------------------------------------------------------------------
	void Camera::setFOVy(const Radian& fov)
	{
		mFOVy = fov;
		invalidateFrustum();
	}

	//-----------------------------------------------------------------------
	const Radian& Camera::getFOVy(void) const
	{
		return mFOVy;
	}


	//-----------------------------------------------------------------------
	void Camera::setFarClipDistance(float farPlane)
	{
		mFarDist = farPlane;
		invalidateFrustum();
	}

	//-----------------------------------------------------------------------
	float Camera::getFarClipDistance(void) const
	{
		return mFarDist;
	}

	//-----------------------------------------------------------------------
	void Camera::setNearClipDistance(float nearPlane)
	{
		if (nearPlane <= 0)
		{
			CM_EXCEPT(InvalidParametersException, "Near clip distance must be greater than zero.");
		}
		mNearDist = nearPlane;
		invalidateFrustum();
	}

	//-----------------------------------------------------------------------
	float Camera::getNearClipDistance(void) const
	{
		return mNearDist;
	}

	//---------------------------------------------------------------------
	void Camera::setFrustumOffset(const Vector2& offset)
	{
		mFrustumOffset = offset;
		invalidateFrustum();
	}
	//---------------------------------------------------------------------
	void Camera::setFrustumOffset(float horizontal, float vertical)
	{
		setFrustumOffset(Vector2(horizontal, vertical));
	}
	//---------------------------------------------------------------------
	const Vector2& Camera::getFrustumOffset() const
	{
		return mFrustumOffset;
	}
	//---------------------------------------------------------------------
	void Camera::setFocalLength(float focalLength)
	{
		if (focalLength <= 0)
		{
			CM_EXCEPT(InvalidParametersException,
				"Focal length must be greater than zero.");
		}

		mFocalLength = focalLength;
		invalidateFrustum();
	}
	//---------------------------------------------------------------------
	float Camera::getFocalLength() const
	{
		return mFocalLength;
	}
	//-----------------------------------------------------------------------
	const Matrix4& Camera::getProjectionMatrix(void) const
	{
		updateFrustum();

		return mProjMatrix;
	}
	//-----------------------------------------------------------------------
	const Matrix4& Camera::getProjectionMatrixWithRSDepth(void) const
	{
		updateFrustum();

		return mProjMatrixRSDepth;
	}
	//-----------------------------------------------------------------------
	const Matrix4& Camera::getProjectionMatrixRS(void) const
	{
		updateFrustum();

		return mProjMatrixRS;
	}
	//-----------------------------------------------------------------------
	const Matrix4& Camera::getViewMatrix(void) const
	{
		updateView();

		return mViewMatrix;

	}

	//-----------------------------------------------------------------------
	const Plane* Camera::getFrustumPlanes(void) const
	{
		// Make any pending updates to the calculated frustum planes
		updateFrustumPlanes();

		return mFrustumPlanes;
	}

	//-----------------------------------------------------------------------
	const Plane& Camera::getFrustumPlane(unsigned short plane) const
	{
		// Make any pending updates to the calculated frustum planes
		updateFrustumPlanes();

		return mFrustumPlanes[plane];

	}

	//-----------------------------------------------------------------------
	bool Camera::isVisible(const AxisAlignedBox& bound, FrustumPlane* culledBy) const
	{
		// Null boxes always invisible
		if (bound.isNull()) return false;

		// Infinite boxes always visible
		if (bound.isInfinite()) return true;

		// Make any pending updates to the calculated frustum planes
		updateFrustumPlanes();

		// Get centre of the box
		Vector3 centre = bound.getCenter();
		// Get the half-size of the box
		Vector3 halfSize = bound.getHalfSize();

		// For each plane, see if all points are on the negative side
		// If so, object is not visible
		for (int plane = 0; plane < 6; ++plane)
		{
			// Skip far plane if infinite view frustum
			if (plane == FRUSTUM_PLANE_FAR && mFarDist == 0)
				continue;

			Plane::Side side = mFrustumPlanes[plane].getSide(centre, halfSize);
			if (side == Plane::NEGATIVE_SIDE)
			{
				// ALL corners on negative side therefore out of view
				if (culledBy)
					*culledBy = (FrustumPlane)plane;
				return false;
			}

		}

		return true;
	}

	//-----------------------------------------------------------------------
	bool Camera::isVisible(const Vector3& vert, FrustumPlane* culledBy) const
	{
		// Make any pending updates to the calculated frustum planes
		updateFrustumPlanes();

		// For each plane, see if all points are on the negative side
		// If so, object is not visible
		for (int plane = 0; plane < 6; ++plane)
		{
			// Skip far plane if infinite view frustum
			if (plane == FRUSTUM_PLANE_FAR && mFarDist == 0)
				continue;

			if (mFrustumPlanes[plane].getSide(vert) == Plane::NEGATIVE_SIDE)
			{
				// ALL corners on negative side therefore out of view
				if (culledBy)
					*culledBy = (FrustumPlane)plane;
				return false;
			}

		}

		return true;
	}
	//-----------------------------------------------------------------------
	bool Camera::isVisible(const Sphere& sphere, FrustumPlane* culledBy) const
	{
		// Make any pending updates to the calculated frustum planes
		updateFrustumPlanes();

		// For each plane, see if sphere is on negative side
		// If so, object is not visible
		for (int plane = 0; plane < 6; ++plane)
		{
			// Skip far plane if infinite view frustum
			if (plane == FRUSTUM_PLANE_FAR && mFarDist == 0)
				continue;

			// If the distance from sphere center to plane is negative, and 'more negative' 
			// than the radius of the sphere, sphere is outside frustum
			if (mFrustumPlanes[plane].getDistance(sphere.getCenter()) < -sphere.getRadius())
			{
				// ALL corners on negative side therefore out of view
				if (culledBy)
					*culledBy = (FrustumPlane)plane;
				return false;
			}

		}

		return true;
	}
	//-----------------------------------------------------------------------
	void Camera::calcProjectionParameters(float& left, float& right, float& bottom, float& top) const
	{ 
		if (mCustomProjMatrix)
		{
			// Convert clipspace corners to camera space
			Matrix4 invProj = mProjMatrix.inverse();
			Vector3 topLeft(-0.5f, 0.5f, 0.0f);
			Vector3 bottomRight(0.5f, -0.5f, 0.0f);

			topLeft = invProj * topLeft;
			bottomRight = invProj * bottomRight;

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
			// Calculate general projection parameters
			else if (mProjType == PT_PERSPECTIVE)
			{
				Radian thetaY (mFOVy * 0.5f);
				float tanThetaY = Math::Tan(thetaY);
				float tanThetaX = tanThetaY * mAspect;

				float nearFocal = mNearDist / mFocalLength;
				float nearOffsetX = mFrustumOffset.x * nearFocal;
				float nearOffsetY = mFrustumOffset.y * nearFocal;
				float half_w = tanThetaX * mNearDist;
				float half_h = tanThetaY * mNearDist;

				left   = - half_w + nearOffsetX;
				right  = + half_w + nearOffsetX;
				bottom = - half_h + nearOffsetY;
				top    = + half_h + nearOffsetY;

				mLeft = left;
				mRight = right;
				mTop = top;
				mBottom = bottom;
			}
			else
			{
				// Unknown how to apply frustum offset to orthographic camera, just ignore here
				float half_w = getOrthoWindowWidth() * 0.5f;
				float half_h = getOrthoWindowHeight() * 0.5f;

				left   = - half_w;
				right  = + half_w;
				bottom = - half_h;
				top    = + half_h;

				mLeft = left;
				mRight = right;
				mTop = top;
				mBottom = bottom;
			}

		}
	}
	//-----------------------------------------------------------------------
	void Camera::updateFrustumImpl(void) const
	{
		// Common calcs
		float left, right, bottom, top;

		calcProjectionParameters(left, right, bottom, top);

		if (!mCustomProjMatrix)
		{

			// The code below will dealing with general projection 
			// parameters, similar glFrustum and glOrtho.
			// Doesn't optimise manually except division operator, so the 
			// code more self-explaining.

			float inv_w = 1 / (right - left);
			float inv_h = 1 / (top - bottom);
			float inv_d = 1 / (mFarDist - mNearDist);

			// Recalc if frustum params changed
			if (mProjType == PT_PERSPECTIVE)
			{
				// Calc matrix elements
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

				// NB: This creates 'uniform' perspective projection matrix,
				// which depth range [-1,1], right-handed rules
				//
				// [ A   0   C   0  ]
				// [ 0   B   D   0  ]
				// [ 0   0   q   qn ]
				// [ 0   0   -1  0  ]
				//
				// A = 2 * near / (right - left)
				// B = 2 * near / (top - bottom)
				// C = (right + left) / (right - left)
				// D = (top + bottom) / (top - bottom)
				// q = - (far + near) / (far - near)
				// qn = - 2 * (far * near) / (far - near)

				mProjMatrix = Matrix4::ZERO;
				mProjMatrix[0][0] = A;
				mProjMatrix[0][2] = C;
				mProjMatrix[1][1] = B;
				mProjMatrix[1][2] = D;
				mProjMatrix[2][2] = q;
				mProjMatrix[2][3] = qn;
				mProjMatrix[3][2] = -1;
			} // perspective
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

				// NB: This creates 'uniform' orthographic projection matrix,
				// which depth range [-1,1], right-handed rules
				//
				// [ A   0   0   C  ]
				// [ 0   B   0   D  ]
				// [ 0   0   q   qn ]
				// [ 0   0   0   1  ]
				//
				// A = 2 * / (right - left)
				// B = 2 * / (top - bottom)
				// C = - (right + left) / (right - left)
				// D = - (top + bottom) / (top - bottom)
				// q = - 2 / (far - near)
				// qn = - (far + near) / (far - near)

				mProjMatrix = Matrix4::ZERO;
				mProjMatrix[0][0] = A;
				mProjMatrix[0][3] = C;
				mProjMatrix[1][1] = B;
				mProjMatrix[1][3] = D;
				mProjMatrix[2][2] = q;
				mProjMatrix[2][3] = qn;
				mProjMatrix[3][3] = 1;
			} // ortho            
		} // !mCustomProjMatrix

		RenderSystem* renderSystem = CamelotEngine::RenderSystem::instancePtr();
		// API specific
		renderSystem->convertProjectionMatrix(mProjMatrix, mProjMatrixRS);
		// API specific for Gpu Programs
		renderSystem->convertProjectionMatrix(mProjMatrix, mProjMatrixRSDepth, true);


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
			min.makeFloor(max);
			max.makeCeil(tmp);
		}

		if (mProjType == PT_PERSPECTIVE)
		{
			// Merge with far plane bounds
			float radio = farDist / mNearDist;
			min.makeFloor(Vector3(left * radio, bottom * radio, -farDist));
			max.makeCeil(Vector3(right * radio, top * radio, 0));
		}
		mBoundingBox.setExtents(min, max);

		mRecalcFrustum = false;

		// Signal to update frustum clipping planes
		mRecalcFrustumPlanes = true;
	}
	//-----------------------------------------------------------------------
	void Camera::updateFrustum(void) const
	{
		if (isFrustumOutOfDate())
		{
			updateFrustumImpl();
		}
	}
	//-----------------------------------------------------------------------
	bool Camera::isFrustumOutOfDate(void) const
	{
		return mRecalcFrustum;
	}
	//-----------------------------------------------------------------------
	void Camera::updateView(void) const
	{
		if (!mCustomViewMatrix)
		{
			Matrix3 rot;
			const Quaternion& orientation = gameObject()->getWorldRotation();
			const Vector3& position = gameObject()->getWorldPosition();

			mViewMatrix = Math::makeViewMatrix(position, orientation, 0);
		}
	}
	//-----------------------------------------------------------------------
	void Camera::updateFrustumPlanesImpl(void) const
	{
		// -------------------------
		// Update the frustum planes
		// -------------------------
		Matrix4 combo = mProjMatrix * mViewMatrix;

		mFrustumPlanes[FRUSTUM_PLANE_LEFT].normal.x = combo[3][0] + combo[0][0];
		mFrustumPlanes[FRUSTUM_PLANE_LEFT].normal.y = combo[3][1] + combo[0][1];
		mFrustumPlanes[FRUSTUM_PLANE_LEFT].normal.z = combo[3][2] + combo[0][2];
		mFrustumPlanes[FRUSTUM_PLANE_LEFT].d = combo[3][3] + combo[0][3];

		mFrustumPlanes[FRUSTUM_PLANE_RIGHT].normal.x = combo[3][0] - combo[0][0];
		mFrustumPlanes[FRUSTUM_PLANE_RIGHT].normal.y = combo[3][1] - combo[0][1];
		mFrustumPlanes[FRUSTUM_PLANE_RIGHT].normal.z = combo[3][2] - combo[0][2];
		mFrustumPlanes[FRUSTUM_PLANE_RIGHT].d = combo[3][3] - combo[0][3];

		mFrustumPlanes[FRUSTUM_PLANE_TOP].normal.x = combo[3][0] - combo[1][0];
		mFrustumPlanes[FRUSTUM_PLANE_TOP].normal.y = combo[3][1] - combo[1][1];
		mFrustumPlanes[FRUSTUM_PLANE_TOP].normal.z = combo[3][2] - combo[1][2];
		mFrustumPlanes[FRUSTUM_PLANE_TOP].d = combo[3][3] - combo[1][3];

		mFrustumPlanes[FRUSTUM_PLANE_BOTTOM].normal.x = combo[3][0] + combo[1][0];
		mFrustumPlanes[FRUSTUM_PLANE_BOTTOM].normal.y = combo[3][1] + combo[1][1];
		mFrustumPlanes[FRUSTUM_PLANE_BOTTOM].normal.z = combo[3][2] + combo[1][2];
		mFrustumPlanes[FRUSTUM_PLANE_BOTTOM].d = combo[3][3] + combo[1][3];

		mFrustumPlanes[FRUSTUM_PLANE_NEAR].normal.x = combo[3][0] + combo[2][0];
		mFrustumPlanes[FRUSTUM_PLANE_NEAR].normal.y = combo[3][1] + combo[2][1];
		mFrustumPlanes[FRUSTUM_PLANE_NEAR].normal.z = combo[3][2] + combo[2][2];
		mFrustumPlanes[FRUSTUM_PLANE_NEAR].d = combo[3][3] + combo[2][3];

		mFrustumPlanes[FRUSTUM_PLANE_FAR].normal.x = combo[3][0] - combo[2][0];
		mFrustumPlanes[FRUSTUM_PLANE_FAR].normal.y = combo[3][1] - combo[2][1];
		mFrustumPlanes[FRUSTUM_PLANE_FAR].normal.z = combo[3][2] - combo[2][2];
		mFrustumPlanes[FRUSTUM_PLANE_FAR].d = combo[3][3] - combo[2][3];

		// Renormalise any normals which were not unit length
		for(int i=0; i<6; i++ ) 
		{
			float length = mFrustumPlanes[i].normal.normalise();
			mFrustumPlanes[i].d /= length;
		}

		mRecalcFrustumPlanes = false;
	}
	//-----------------------------------------------------------------------
	void Camera::updateFrustumPlanes(void) const
	{
		updateView();
		updateFrustum();

		if (mRecalcFrustumPlanes)
		{
			updateFrustumPlanesImpl();
		}
	}
	//-----------------------------------------------------------------------
	void Camera::updateWorldSpaceCornersImpl(void) const
	{
		Matrix4 eyeToWorld = mViewMatrix.inverseAffine();

		// Note: Even though we can dealing with general projection matrix here,
		//       but because it's incompatibly with infinite far plane, thus, we
		//       still need to working with projection parameters.

		// Calc near plane corners
		float nearLeft, nearRight, nearBottom, nearTop;
		calcProjectionParameters(nearLeft, nearRight, nearBottom, nearTop);

		// Treat infinite fardist as some arbitrary far value
		float farDist = (mFarDist == 0) ? 100000 : mFarDist;

		// Calc far palne corners
		float radio = mProjType == PT_PERSPECTIVE ? farDist / mNearDist : 1;
		float farLeft = nearLeft * radio;
		float farRight = nearRight * radio;
		float farBottom = nearBottom * radio;
		float farTop = nearTop * radio;

		// near
		mWorldSpaceCorners[0] = eyeToWorld.transformAffine(Vector3(nearRight, nearTop,    -mNearDist));
		mWorldSpaceCorners[1] = eyeToWorld.transformAffine(Vector3(nearLeft,  nearTop,    -mNearDist));
		mWorldSpaceCorners[2] = eyeToWorld.transformAffine(Vector3(nearLeft,  nearBottom, -mNearDist));
		mWorldSpaceCorners[3] = eyeToWorld.transformAffine(Vector3(nearRight, nearBottom, -mNearDist));
		// far
		mWorldSpaceCorners[4] = eyeToWorld.transformAffine(Vector3(farRight,  farTop,     -farDist));
		mWorldSpaceCorners[5] = eyeToWorld.transformAffine(Vector3(farLeft,   farTop,     -farDist));
		mWorldSpaceCorners[6] = eyeToWorld.transformAffine(Vector3(farLeft,   farBottom,  -farDist));
		mWorldSpaceCorners[7] = eyeToWorld.transformAffine(Vector3(farRight,  farBottom,  -farDist));


		mRecalcWorldSpaceCorners = false;
	}
	//-----------------------------------------------------------------------
	void Camera::updateWorldSpaceCorners(void) const
	{
		updateView();

		if (mRecalcWorldSpaceCorners)
		{
			updateWorldSpaceCornersImpl();
		}

	}
	//-----------------------------------------------------------------------
	float Camera::getAspectRatio(void) const
	{
		return mAspect;
	}
	//-----------------------------------------------------------------------
	void Camera::setAspectRatio(float r)
	{
		mAspect = r;
		invalidateFrustum();
	}
	//-----------------------------------------------------------------------
	const AxisAlignedBox& Camera::getBoundingBox(void) const
	{
		return mBoundingBox;
	}
	// -------------------------------------------------------------------
	const Vector3* Camera::getWorldSpaceCorners(void) const
	{
		updateWorldSpaceCorners();

		return mWorldSpaceCorners;
	}
	//-----------------------------------------------------------------------
	void Camera::setProjectionType(ProjectionType pt)
	{
		mProjType = pt;
		invalidateFrustum();
	}

	//-----------------------------------------------------------------------
	ProjectionType Camera::getProjectionType(void) const
	{
		return mProjType;
	}
	//---------------------------------------------------------------------
	bool Camera::projectSphere(const Sphere& sphere, 
		float* left, float* top, float* right, float* bottom) const
	{
		// See http://www.gamasutra.com/features/20021011/lengyel_06.htm
		// Transform light position into camera space

		updateView();
		Vector3 eyeSpacePos = mViewMatrix.transformAffine(sphere.getCenter());

		// initialise
		*left = *bottom = -1.0f;
		*right = *top = 1.0f;

		if (eyeSpacePos.z < 0)
		{
			updateFrustum();
			const Matrix4& projMatrix = getProjectionMatrix();
			float r = sphere.getRadius();
			float rsq = r * r;

			// early-exit
			if (eyeSpacePos.squaredLength() <= rsq)
				return false;

			float Lxz = Math::Sqr(eyeSpacePos.x) + Math::Sqr(eyeSpacePos.z);
			float Lyz = Math::Sqr(eyeSpacePos.y) + Math::Sqr(eyeSpacePos.z);

			// Find the tangent planes to the sphere
			// XZ first
			// calculate quadratic discriminant: b*b - 4ac
			// x = Nx
			// a = Lx^2 + Lz^2
			// b = -2rLx
			// c = r^2 - Lz^2
			float a = Lxz;
			float b = -2.0f * r * eyeSpacePos.x;
			float c = rsq - Math::Sqr(eyeSpacePos.z);
			float D = b*b - 4.0f*a*c;

			// two roots?
			if (D > 0)
			{
				float sqrootD = Math::Sqrt(D);
				// solve the quadratic to get the components of the normal
				float Nx0 = (-b + sqrootD) / (2 * a);
				float Nx1 = (-b - sqrootD) / (2 * a);

				// Derive Z from this
				float Nz0 = (r - Nx0 * eyeSpacePos.x) / eyeSpacePos.z;
				float Nz1 = (r - Nx1 * eyeSpacePos.x) / eyeSpacePos.z;

				// Get the point of tangency
				// Only consider points of tangency in front of the camera
				float Pz0 = (Lxz - rsq) / (eyeSpacePos.z - ((Nz0 / Nx0) * eyeSpacePos.x));
				if (Pz0 < 0)
				{
					// Project point onto near plane in worldspace
					float nearx0 = (Nz0 * mNearDist) / Nx0;
					// now we need to map this to viewport coords
					// use projection matrix since that will take into account all factors
					Vector3 relx0 = projMatrix * Vector3(nearx0, 0, -mNearDist);

					// find out whether this is a left side or right side
					float Px0 = -(Pz0 * Nz0) / Nx0;
					if (Px0 > eyeSpacePos.x)
					{
						*right = std::min(*right, relx0.x);
					}
					else
					{
						*left = std::max(*left, relx0.x);
					}
				}
				float Pz1 = (Lxz - rsq) / (eyeSpacePos.z - ((Nz1 / Nx1) * eyeSpacePos.x));
				if (Pz1 < 0)
				{
					// Project point onto near plane in worldspace
					float nearx1 = (Nz1 * mNearDist) / Nx1;
					// now we need to map this to viewport coords
					// use projection matrix since that will take into account all factors
					Vector3 relx1 = projMatrix * Vector3(nearx1, 0, -mNearDist);

					// find out whether this is a left side or right side
					float Px1 = -(Pz1 * Nz1) / Nx1;
					if (Px1 > eyeSpacePos.x)
					{
						*right = std::min(*right, relx1.x);
					}
					else
					{
						*left = std::max(*left, relx1.x);
					}
				}
			}


			// Now YZ 
			// calculate quadratic discriminant: b*b - 4ac
			// x = Ny
			// a = Ly^2 + Lz^2
			// b = -2rLy
			// c = r^2 - Lz^2
			a = Lyz;
			b = -2.0f * r * eyeSpacePos.y;
			c = rsq - Math::Sqr(eyeSpacePos.z);
			D = b*b - 4.0f*a*c;

			// two roots?
			if (D > 0)
			{
				float sqrootD = Math::Sqrt(D);
				// solve the quadratic to get the components of the normal
				float Ny0 = (-b + sqrootD) / (2 * a);
				float Ny1 = (-b - sqrootD) / (2 * a);

				// Derive Z from this
				float Nz0 = (r - Ny0 * eyeSpacePos.y) / eyeSpacePos.z;
				float Nz1 = (r - Ny1 * eyeSpacePos.y) / eyeSpacePos.z;

				// Get the point of tangency
				// Only consider points of tangency in front of the camera
				float Pz0 = (Lyz - rsq) / (eyeSpacePos.z - ((Nz0 / Ny0) * eyeSpacePos.y));
				if (Pz0 < 0)
				{
					// Project point onto near plane in worldspace
					float neary0 = (Nz0 * mNearDist) / Ny0;
					// now we need to map this to viewport coords
					// use projection matriy since that will take into account all factors
					Vector3 rely0 = projMatrix * Vector3(0, neary0, -mNearDist);

					// find out whether this is a top side or bottom side
					float Py0 = -(Pz0 * Nz0) / Ny0;
					if (Py0 > eyeSpacePos.y)
					{
						*top = std::min(*top, rely0.y);
					}
					else
					{
						*bottom = std::max(*bottom, rely0.y);
					}
				}
				float Pz1 = (Lyz - rsq) / (eyeSpacePos.z - ((Nz1 / Ny1) * eyeSpacePos.y));
				if (Pz1 < 0)
				{
					// Project point onto near plane in worldspace
					float neary1 = (Nz1 * mNearDist) / Ny1;
					// now we need to map this to viewport coords
					// use projection matriy since that will take into account all factors
					Vector3 rely1 = projMatrix * Vector3(0, neary1, -mNearDist);

					// find out whether this is a top side or bottom side
					float Py1 = -(Pz1 * Nz1) / Ny1;
					if (Py1 > eyeSpacePos.y)
					{
						*top = std::min(*top, rely1.y);
					}
					else
					{
						*bottom = std::max(*bottom, rely1.y);
					}
				}
			}
		}

		return (*left != -1.0f) || (*top != 1.0f) || (*right != 1.0f) || (*bottom != -1.0f);

	}
	//---------------------------------------------------------------------
	void Camera::setCustomViewMatrix(bool enable, const Matrix4& viewMatrix)
	{
		mCustomViewMatrix = enable;
		if (enable)
		{
			assert(viewMatrix.isAffine());
			mViewMatrix = viewMatrix;
		}
	}
	//---------------------------------------------------------------------
	void Camera::setCustomProjectionMatrix(bool enable, const Matrix4& projMatrix)
	{
		mCustomProjMatrix = enable;
		if (enable)
		{
			mProjMatrix = projMatrix;
		}
		invalidateFrustum();
	}
	//---------------------------------------------------------------------
	void Camera::setOrthoWindow(float w, float h)
	{
		mOrthoHeight = h;
		mAspect = w / h;
		invalidateFrustum();
	}
	//---------------------------------------------------------------------
	void Camera::setOrthoWindowHeight(float h)
	{
		mOrthoHeight = h;
		invalidateFrustum();
	}
	//---------------------------------------------------------------------
	void Camera::setOrthoWindowWidth(float w)
	{
		mOrthoHeight = w / mAspect;
		invalidateFrustum();
	}
	//---------------------------------------------------------------------
	float Camera::getOrthoWindowHeight() const
	{
		return mOrthoHeight;
	}
	//---------------------------------------------------------------------
	float Camera::getOrthoWindowWidth() const
	{
		return mOrthoHeight * mAspect;	
	}
	//---------------------------------------------------------------------
	void Camera::setFrustumExtents(float left, float right, float top, float bottom)
	{
		mFrustumExtentsManuallySet = true;
		mLeft = left;
		mRight = right;
		mTop = top;
		mBottom = bottom;

		invalidateFrustum();
	}
	//---------------------------------------------------------------------
	void Camera::resetFrustumExtents()
	{
		mFrustumExtentsManuallySet = false;
		invalidateFrustum();
	}
	//---------------------------------------------------------------------
	void Camera::getFrustumExtents(float& outleft, float& outright, float& outtop, float& outbottom) const
	{
		updateFrustum();
		outleft = mLeft;
		outright = mRight;
		outtop = mTop;
		outbottom = mBottom;
	}
    // -------------------------------------------------------------------
    void Camera::invalidateFrustum(void) const
    {
		mRecalcFrustumPlanes = true;
		mRecalcWorldSpaceCorners = true;
		mRecalcVertexData = true;
    }
    // -------------------------------------------------------------------
    float Camera::getBoundingRadius(void) const
    {
        // return a little bigger than the near distance
        // just to keep things just outside
        return mNearDist * 1.5f;

    }

	RTTITypeBase* Camera::getRTTIStatic()
	{
		return CameraRTTI::instance();
	}

	RTTITypeBase* Camera::getRTTI() const
	{
		return Camera::getRTTIStatic();
	}
} // namespace CamelotEngine

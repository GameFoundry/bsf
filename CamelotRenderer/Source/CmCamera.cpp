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

#include "CmMath.h"
#include "CmMatrix3.h"
#include "CmVector2.h"
#include "CmAxisAlignedBox.h"
#include "CmSphere.h"
#include "CmHardwareBufferManager.h"
#include "CmHardwareVertexBuffer.h"
#include "CmHardwareIndexBuffer.h"
#include "CmRenderSystemManager.h"
#include "CmException.h"
#include "CmRenderSystem.h"

#if CM_PLATFORM == OGRE_PLATFORM_IPHONE
#include "macUtils.h"
#endif

namespace CamelotEngine {
	const float Camera::INFINITE_FAR_PLANE_ADJUST = 0.00001f;

    //-----------------------------------------------------------------------
	Camera::Camera(RenderTarget* target,
		float left, float top,
		float width, float height,
		int ZOrder)
        : mProjType(PT_PERSPECTIVE), 
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
		mRecalcView(true), 
		mRecalcFrustumPlanes(true),
		mRecalcWorldSpaceCorners(true),
		mRecalcVertexData(true),
		mCustomViewMatrix(false),
		mCustomProjMatrix(false),
		mFrustumExtentsManuallySet(false),
		mOrientation(Quaternion::IDENTITY),
		mPosition(Vector3::ZERO),
		mSceneDetail(PM_SOLID),
		mWindowSet(false),
		mLastViewport(0),
		mAutoAspectRatio(false)
    {
		updateView();
		updateFrustum();

        // Reasonable defaults to camera params
        mFOVy = Radian(Math::PI/4.0f);
        mNearDist = 100.0f;
        mFarDist = 100000.0f;
        mAspect = 1.33333333333333f;
        mProjType = PT_PERSPECTIVE;
        setFixedYawAxis(true);    // Default to fixed yaw, like freelook since most people expect this

        invalidateFrustum();
        invalidateView();

        // Init matrices
        mViewMatrix = Matrix4::ZERO;
        mProjMatrixRS = Matrix4::ZERO;

		mViewport = new Viewport(target, left, top, width, height, ZOrder);
    }

    //-----------------------------------------------------------------------
    Camera::~Camera()
    {
		if(mViewport != nullptr)
			delete mViewport;
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

		RenderSystem* renderSystem = CamelotEngine::RenderSystemManager::getActive();
		// API specific
		renderSystem->_convertProjectionMatrix(mProjMatrix, mProjMatrixRS);
		// API specific for Gpu Programs
		renderSystem->_convertProjectionMatrix(mProjMatrix, mProjMatrixRSDepth, true);


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
	void Camera::updateVertexData(void) const
	{
		if (mRecalcVertexData)
		{
			if (mVertexData.vertexBufferBinding->getBufferCount() <= 0)
			{
				// Initialise vertex & index data
				mVertexData.vertexDeclaration->addElement(0, 0, VET_FLOAT3, VES_POSITION);
				mVertexData.vertexCount = 32;
				mVertexData.vertexStart = 0;
				mVertexData.vertexBufferBinding->setBinding( 0,
					HardwareBufferManager::instance().createVertexBuffer(
					sizeof(float)*3, 32, HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY) );
			}

			// Note: Even though we can dealing with general projection matrix here,
			//       but because it's incompatibly with infinite far plane, thus, we
			//       still need to working with projection parameters.

			// Calc near plane corners
			float vpLeft, vpRight, vpBottom, vpTop;
			calcProjectionParameters(vpLeft, vpRight, vpBottom, vpTop);

			// Treat infinite fardist as some arbitrary far value
			float farDist = (mFarDist == 0) ? 100000 : mFarDist;

			// Calc far plane corners
			float radio = mProjType == PT_PERSPECTIVE ? farDist / mNearDist : 1;
			float farLeft = vpLeft * radio;
			float farRight = vpRight * radio;
			float farBottom = vpBottom * radio;
			float farTop = vpTop * radio;

			// Calculate vertex positions (local)
			// 0 is the origin
			// 1, 2, 3, 4 are the points on the near plane, top left first, clockwise
			// 5, 6, 7, 8 are the points on the far plane, top left first, clockwise
			HardwareVertexBufferPtr vbuf = mVertexData.vertexBufferBinding->getBuffer(0);
			float* pFloat = static_cast<float*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));

			// near plane (remember frustum is going in -Z direction)
			*pFloat++ = vpLeft;  *pFloat++ = vpTop;    *pFloat++ = -mNearDist;
			*pFloat++ = vpRight; *pFloat++ = vpTop;    *pFloat++ = -mNearDist;

			*pFloat++ = vpRight; *pFloat++ = vpTop;    *pFloat++ = -mNearDist;
			*pFloat++ = vpRight; *pFloat++ = vpBottom; *pFloat++ = -mNearDist;

			*pFloat++ = vpRight; *pFloat++ = vpBottom; *pFloat++ = -mNearDist;
			*pFloat++ = vpLeft;  *pFloat++ = vpBottom; *pFloat++ = -mNearDist;

			*pFloat++ = vpLeft;  *pFloat++ = vpBottom; *pFloat++ = -mNearDist;
			*pFloat++ = vpLeft;  *pFloat++ = vpTop;    *pFloat++ = -mNearDist;

			// far plane (remember frustum is going in -Z direction)
			*pFloat++ = farLeft;  *pFloat++ = farTop;    *pFloat++ = -farDist;
			*pFloat++ = farRight; *pFloat++ = farTop;    *pFloat++ = -farDist;

			*pFloat++ = farRight; *pFloat++ = farTop;    *pFloat++ = -farDist;
			*pFloat++ = farRight; *pFloat++ = farBottom; *pFloat++ = -farDist;

			*pFloat++ = farRight; *pFloat++ = farBottom; *pFloat++ = -farDist;
			*pFloat++ = farLeft;  *pFloat++ = farBottom; *pFloat++ = -farDist;

			*pFloat++ = farLeft;  *pFloat++ = farBottom; *pFloat++ = -farDist;
			*pFloat++ = farLeft;  *pFloat++ = farTop;    *pFloat++ = -farDist;

			// Sides of the pyramid
			*pFloat++ = 0.0f;    *pFloat++ = 0.0f;   *pFloat++ = 0.0f;
			*pFloat++ = vpLeft;  *pFloat++ = vpTop;  *pFloat++ = -mNearDist;

			*pFloat++ = 0.0f;    *pFloat++ = 0.0f;   *pFloat++ = 0.0f;
			*pFloat++ = vpRight; *pFloat++ = vpTop;    *pFloat++ = -mNearDist;

			*pFloat++ = 0.0f;    *pFloat++ = 0.0f;   *pFloat++ = 0.0f;
			*pFloat++ = vpRight; *pFloat++ = vpBottom; *pFloat++ = -mNearDist;

			*pFloat++ = 0.0f;    *pFloat++ = 0.0f;   *pFloat++ = 0.0f;
			*pFloat++ = vpLeft;  *pFloat++ = vpBottom; *pFloat++ = -mNearDist;

			// Sides of the box

			*pFloat++ = vpLeft;  *pFloat++ = vpTop;  *pFloat++ = -mNearDist;
			*pFloat++ = farLeft;  *pFloat++ = farTop;  *pFloat++ = -farDist;

			*pFloat++ = vpRight; *pFloat++ = vpTop;    *pFloat++ = -mNearDist;
			*pFloat++ = farRight; *pFloat++ = farTop;    *pFloat++ = -farDist;

			*pFloat++ = vpRight; *pFloat++ = vpBottom; *pFloat++ = -mNearDist;
			*pFloat++ = farRight; *pFloat++ = farBottom; *pFloat++ = -farDist;

			*pFloat++ = vpLeft;  *pFloat++ = vpBottom; *pFloat++ = -mNearDist;
			*pFloat++ = farLeft;  *pFloat++ = farBottom; *pFloat++ = -farDist;


			vbuf->unlock();

			mRecalcVertexData = false;
		}
	}
	//-----------------------------------------------------------------------
	bool Camera::isFrustumOutOfDate(void) const
	{
		return mRecalcFrustum;
	}

	//-----------------------------------------------------------------------
	void Camera::updateViewImpl(void) const
	{
		// ----------------------
		// Update the view matrix
		// ----------------------

		// Get orientation from quaternion

		if (!mCustomViewMatrix)
		{
			Matrix3 rot;
			const Quaternion& orientation = getOrientationForViewUpdate();
			const Vector3& position = getPositionForViewUpdate();

			mViewMatrix = Math::makeViewMatrix(position, orientation, 0);
		}

		mRecalcView = false;

		// Signal to update frustum clipping planes
		mRecalcFrustumPlanes = true;
		// Signal to update world space corners
		mRecalcWorldSpaceCorners = true;
	}
	//---------------------------------------------------------------------
	void Camera::calcViewMatrixRelative(const Vector3& relPos, Matrix4& matToUpdate) const
	{
		Matrix4 matTrans = Matrix4::IDENTITY;
		matTrans.setTrans(relPos);
		matToUpdate = getViewMatrix() * matTrans;

	}
	//-----------------------------------------------------------------------
	void Camera::updateView(void) const
	{
		if (isViewOutOfDate())
		{
			updateViewImpl();
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
		invalidateView();
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
    //-----------------------------------------------------------------------
    void Camera::setPolygonMode(PolygonMode sd)
    {
        mSceneDetail = sd;
    }

    //-----------------------------------------------------------------------
    PolygonMode Camera::getPolygonMode(void) const
    {
        return mSceneDetail;
    }

    //-----------------------------------------------------------------------
    void Camera::setPosition(float x, float y, float z)
    {
        mPosition.x = x;
        mPosition.y = y;
        mPosition.z = z;
        invalidateView();
    }

    //-----------------------------------------------------------------------
    void Camera::setPosition(const Vector3& vec)
    {
        mPosition = vec;
        invalidateView();
    }

    //-----------------------------------------------------------------------
    const Vector3& Camera::getPosition(void) const
    {
        return mPosition;
    }

    //-----------------------------------------------------------------------
    void Camera::move(const Vector3& vec)
    {
        mPosition = mPosition + vec;
        invalidateView();
    }

    //-----------------------------------------------------------------------
    void Camera::moveRelative(const Vector3& vec)
    {
        // Transform the axes of the relative vector by camera's local axes
        Vector3 trans = mOrientation * vec;

        mPosition = mPosition + trans;
        invalidateView();
    }

    //-----------------------------------------------------------------------
    void Camera::setDirection(float x, float y, float z)
    {
        setDirection(Vector3(x,y,z));
    }

    //-----------------------------------------------------------------------
    void Camera::setDirection(const Vector3& vec)
    {
        // Do nothing if given a zero vector
        // (Replaced assert since this could happen with auto tracking camera and
        //  camera passes through the lookAt point)
        if (vec == Vector3::ZERO) return;

        // Remember, camera points down -Z of local axes!
        // Therefore reverse direction of direction vector before determining local Z
        Vector3 zAdjustVec = -vec;
        zAdjustVec.normalise();

		Quaternion targetWorldOrientation;


        if( mYawFixed )
        {
            Vector3 xVec = mYawFixedAxis.crossProduct( zAdjustVec );
            xVec.normalise();

            Vector3 yVec = zAdjustVec.crossProduct( xVec );
            yVec.normalise();

            targetWorldOrientation.FromAxes( xVec, yVec, zAdjustVec );
        }
        else
        {

            // Get axes from current quaternion
            Vector3 axes[3];
            updateView();
            mRealOrientation.ToAxes(axes);
            Quaternion rotQuat;
            if ( (axes[2]+zAdjustVec).squaredLength() <  0.00005f) 
            {
                // Oops, a 180 degree turn (infinite possible rotation axes)
                // Default to yaw i.e. use current UP
                rotQuat.FromAngleAxis(Radian(Math::PI), axes[1]);
            }
            else
            {
                // Derive shortest arc to new direction
                rotQuat = axes[2].getRotationTo(zAdjustVec);

            }
            targetWorldOrientation = rotQuat * mRealOrientation;
        }

        // transform to parent space
		// TODO PORT -  Can't get orientation from parent until we hook it up properly as a Component
  //      if (mParentNode)
  //      {
  //          mOrientation =
  //              mParentNode->_getDerivedOrientation().Inverse() * targetWorldOrientation;
  //      }
		//else
		{
			mOrientation = targetWorldOrientation;
		}

        // TODO If we have a fixed yaw axis, we mustn't break it by using the
        // shortest arc because this will sometimes cause a relative yaw
        // which will tip the camera

        invalidateView();

    }

    //-----------------------------------------------------------------------
    Vector3 Camera::getDirection(void) const
    {
        // Direction points down -Z by default
        return mOrientation * -Vector3::UNIT_Z;
    }

    //-----------------------------------------------------------------------
    Vector3 Camera::getUp(void) const
    {
        return mOrientation * Vector3::UNIT_Y;
    }

    //-----------------------------------------------------------------------
    Vector3 Camera::getRight(void) const
    {
        return mOrientation * Vector3::UNIT_X;
    }

    //-----------------------------------------------------------------------
    void Camera::lookAt(const Vector3& targetPoint)
    {
        updateView();
        this->setDirection(targetPoint - mRealPosition);
    }

    //-----------------------------------------------------------------------
    void Camera::lookAt( float x, float y, float z )
    {
        Vector3 vTemp( x, y, z );
        this->lookAt(vTemp);
    }

    //-----------------------------------------------------------------------
    void Camera::roll(const Radian& angle)
    {
        // Rotate around local Z axis
        Vector3 zAxis = mOrientation * Vector3::UNIT_Z;
        rotate(zAxis, angle);

        invalidateView();
    }

    //-----------------------------------------------------------------------
    void Camera::yaw(const Radian& angle)
    {
        Vector3 yAxis;

        if (mYawFixed)
        {
            // Rotate around fixed yaw axis
            yAxis = mYawFixedAxis;
        }
        else
        {
            // Rotate around local Y axis
            yAxis = mOrientation * Vector3::UNIT_Y;
        }

        rotate(yAxis, angle);

        invalidateView();
    }

    //-----------------------------------------------------------------------
    void Camera::pitch(const Radian& angle)
    {
        // Rotate around local X axis
        Vector3 xAxis = mOrientation * Vector3::UNIT_X;
        rotate(xAxis, angle);

        invalidateView();

    }

    //-----------------------------------------------------------------------
    void Camera::rotate(const Vector3& axis, const Radian& angle)
    {
        Quaternion q;
        q.FromAngleAxis(angle,axis);
        rotate(q);
    }

    //-----------------------------------------------------------------------
    void Camera::rotate(const Quaternion& q)
    {
        // Note the order of the mult, i.e. q comes after

		// Normalise the quat to avoid cumulative problems with precision
		Quaternion qnorm = q;
		qnorm.normalise();
        mOrientation = qnorm * mOrientation;

        invalidateView();

    }

    //-----------------------------------------------------------------------
    bool Camera::isViewOutOfDate(void) const
    {
        // Overridden from Frustum to use local orientation / position offsets
        // Attached to node?
		// TODO PORT -  Can't get orientation/position from parent until we hook it up properly as a Component
        //if (mParentNode != 0)
        //{
        //    if (mRecalcView ||
        //        mParentNode->_getDerivedOrientation() != mLastParentOrientation ||
        //        mParentNode->_getDerivedPosition() != mLastParentPosition)
        //    {
        //        // Ok, we're out of date with SceneNode we're attached to
        //        mLastParentOrientation = mParentNode->_getDerivedOrientation();
        //        mLastParentPosition = mParentNode->_getDerivedPosition();
        //        mRealOrientation = mLastParentOrientation * mOrientation;
        //        mRealPosition = (mLastParentOrientation * mPosition) + mLastParentPosition;
        //        mRecalcView = true;
        //        mRecalcWindow = true;
        //    }
        //}
        //else
        {
            // Rely on own updates
            mRealOrientation = mOrientation;
            mRealPosition = mPosition;
        }

        // Deriving reflected orientation / position
        if (mRecalcView)
        {
			mDerivedOrientation = mRealOrientation;
			mDerivedPosition = mRealPosition;
        }

        return mRecalcView;

    }

    // -------------------------------------------------------------------
    void Camera::invalidateView() const
    {
        mRecalcWindow = true;
		mRecalcView = true;
		mRecalcFrustumPlanes = true;
		mRecalcWorldSpaceCorners = true;
    }
    // -------------------------------------------------------------------
    void Camera::invalidateFrustum(void) const
    {
        mRecalcWindow = true;
		mRecalcFrustum = true;
		mRecalcFrustumPlanes = true;
		mRecalcWorldSpaceCorners = true;
		mRecalcVertexData = true;
    }
    //-----------------------------------------------------------------------
    void Camera::_renderScene(Viewport *vp, bool includeOverlays)
    {
		// TODO PORT - I'm not going to be rendering the scene like this (yet), but I think I will do it eventually
        //mSceneMgr->_renderScene(this, vp, includeOverlays);
	}

    //-----------------------------------------------------------------------
    void Camera::setFixedYawAxis(bool useFixed, const Vector3& fixedAxis)
    {
        mYawFixed = useFixed;
        mYawFixedAxis = fixedAxis;
    }
    //-----------------------------------------------------------------------
    const Quaternion& Camera::getOrientation(void) const
    {
        return mOrientation;
    }

    //-----------------------------------------------------------------------
    void Camera::setOrientation(const Quaternion& q)
    {
        mOrientation = q;
		mOrientation.normalise();
        invalidateView();
    }
    //-----------------------------------------------------------------------
    const Quaternion& Camera::getDerivedOrientation(void) const
    {
        updateView();
        return mDerivedOrientation;
    }
    //-----------------------------------------------------------------------
    const Vector3& Camera::getDerivedPosition(void) const
    {
        updateView();
        return mDerivedPosition;
    }
    //-----------------------------------------------------------------------
    Vector3 Camera::getDerivedDirection(void) const
    {
        // Direction points down -Z
        updateView();
        return mDerivedOrientation * Vector3::NEGATIVE_UNIT_Z;
    }
    //-----------------------------------------------------------------------
    Vector3 Camera::getDerivedUp(void) const
    {
        updateView();
        return mDerivedOrientation * Vector3::UNIT_Y;
    }
    //-----------------------------------------------------------------------
    Vector3 Camera::getDerivedRight(void) const
    {
        updateView();
        return mDerivedOrientation * Vector3::UNIT_X;
    }
    //-----------------------------------------------------------------------
    const Quaternion& Camera::getRealOrientation(void) const
    {
        updateView();
        return mRealOrientation;
    }
    //-----------------------------------------------------------------------
    const Vector3& Camera::getRealPosition(void) const
    {
        updateView();
        return mRealPosition;
    }
    //-----------------------------------------------------------------------
    Vector3 Camera::getRealDirection(void) const
    {
        // Direction points down -Z
        updateView();
        return mRealOrientation * Vector3::NEGATIVE_UNIT_Z;
    }
    //-----------------------------------------------------------------------
    Vector3 Camera::getRealUp(void) const
    {
        updateView();
        return mRealOrientation * Vector3::UNIT_Y;
    }
    //-----------------------------------------------------------------------
    Vector3 Camera::getRealRight(void) const
    {
        updateView();
        return mRealOrientation * Vector3::UNIT_X;
    }
    //-----------------------------------------------------------------------
	Ray Camera::getCameraToViewportRay(float screenX, float screenY) const
	{
		Ray ret;
		getCameraToViewportRay(screenX, screenY, &ret);
		return ret;
	}
	//---------------------------------------------------------------------
    void Camera::getCameraToViewportRay(float screenX, float screenY, Ray* outRay) const
    {
		Matrix4 inverseVP = (getProjectionMatrix() * getViewMatrix()).inverse();

		float nx = (2.0f * screenX) - 1.0f;
		float ny = 1.0f - (2.0f * screenY);
		Vector3 nearPoint(nx, ny, -1.f);
		// Use midPoint rather than far point to avoid issues with infinite projection
		Vector3 midPoint (nx, ny,  0.0f);

		// Get ray origin and ray target on near plane in world space
		Vector3 rayOrigin, rayTarget;
		
		rayOrigin = inverseVP * nearPoint;
		rayTarget = inverseVP * midPoint;

		Vector3 rayDirection = rayTarget - rayOrigin;
		rayDirection.normalise();

		outRay->setOrigin(rayOrigin);
		outRay->setDirection(rayDirection);
    } 
    // -------------------------------------------------------------------
    void Camera::setWindow (float Left, float Top, float Right, float Bottom)
    {
        mWLeft = Left;
        mWTop = Top;
        mWRight = Right;
        mWBottom = Bottom;

        mWindowSet = true;
        mRecalcWindow = true;
    }
    // -------------------------------------------------------------------
    void Camera::resetWindow ()
    {
        mWindowSet = false;
    }
    // -------------------------------------------------------------------
    void Camera::setWindowImpl() const
    {
        if (!mWindowSet || !mRecalcWindow)
            return;

        // Calculate general projection parameters
        float vpLeft, vpRight, vpBottom, vpTop;
        calcProjectionParameters(vpLeft, vpRight, vpBottom, vpTop);

        float vpWidth = vpRight - vpLeft;
        float vpHeight = vpTop - vpBottom;

        float wvpLeft   = vpLeft + mWLeft * vpWidth;
        float wvpRight  = vpLeft + mWRight * vpWidth;
        float wvpTop    = vpTop - mWTop * vpHeight;
        float wvpBottom = vpTop - mWBottom * vpHeight;

        Vector3 vp_ul (wvpLeft, wvpTop, -mNearDist);
        Vector3 vp_ur (wvpRight, wvpTop, -mNearDist);
        Vector3 vp_bl (wvpLeft, wvpBottom, -mNearDist);
        Vector3 vp_br (wvpRight, wvpBottom, -mNearDist);

        Matrix4 inv = mViewMatrix.inverseAffine();

        Vector3 vw_ul = inv.transformAffine(vp_ul);
        Vector3 vw_ur = inv.transformAffine(vp_ur);
        Vector3 vw_bl = inv.transformAffine(vp_bl);
        Vector3 vw_br = inv.transformAffine(vp_br);

		mWindowClipPlanes.clear();
        if (mProjType == PT_PERSPECTIVE)
        {
            Vector3 position = getPositionForViewUpdate();
            mWindowClipPlanes.push_back(Plane(position, vw_bl, vw_ul));
            mWindowClipPlanes.push_back(Plane(position, vw_ul, vw_ur));
            mWindowClipPlanes.push_back(Plane(position, vw_ur, vw_br));
            mWindowClipPlanes.push_back(Plane(position, vw_br, vw_bl));
        }
        else
        {
            Vector3 x_axis(inv[0][0], inv[0][1], inv[0][2]);
            Vector3 y_axis(inv[1][0], inv[1][1], inv[1][2]);
            x_axis.normalise();
            y_axis.normalise();
            mWindowClipPlanes.push_back(Plane( x_axis, vw_bl));
            mWindowClipPlanes.push_back(Plane(-x_axis, vw_ur));
            mWindowClipPlanes.push_back(Plane( y_axis, vw_bl));
            mWindowClipPlanes.push_back(Plane(-y_axis, vw_ur));
        }

        mRecalcWindow = false;

    }
    // -------------------------------------------------------------------
    const vector<Plane>::type& Camera::getWindowPlanes(void) const
    {
        updateView();
        setWindowImpl();
        return mWindowClipPlanes;
    }
    // -------------------------------------------------------------------
    float Camera::getBoundingRadius(void) const
    {
        // return a little bigger than the near distance
        // just to keep things just outside
        return mNearDist * 1.5f;

    }
    //-----------------------------------------------------------------------
    const Vector3& Camera::getPositionForViewUpdate(void) const
    {
        // Note no update, because we're calling this from the update!
        return mRealPosition;
    }
    //-----------------------------------------------------------------------
    const Quaternion& Camera::getOrientationForViewUpdate(void) const
    {
        return mRealOrientation;
    }
    //-----------------------------------------------------------------------
    bool Camera::getAutoAspectRatio(void) const
    {
        return mAutoAspectRatio;
    }
    //-----------------------------------------------------------------------
    void Camera::setAutoAspectRatio(bool autoratio)
    {
        mAutoAspectRatio = autoratio;
    }
	//-----------------------------------------------------------------------
	//_______________________________________________________
	//|														|
	//|	getRayForwardIntersect								|
	//|	-----------------------------						|
	//|	get the intersections of frustum rays with a plane	|
	//| of interest.  The plane is assumed to have constant	|
	//| z.  If this is not the case, rays					|
	//| should be rotated beforehand to work in a			|
	//| coordinate system in which this is true.			|
	//|_____________________________________________________|
	//
	vector<Vector4>::type Camera::getRayForwardIntersect(const Vector3& anchor, const Vector3 *dir, float planeOffset) const
	{
		vector<Vector4>::type res;

		if(!dir)
			return res;

		int infpt[4] = {0, 0, 0, 0}; // 0=finite, 1=infinite, 2=straddles infinity
		Vector3 vec[4];

		// find how much the anchor point must be displaced in the plane's
		// constant variable
		float delta = planeOffset - anchor.z;

		// now set the intersection point and note whether it is a 
		// point at infinity or straddles infinity
		unsigned int i;
		for (i=0; i<4; i++)
		{
			float test = dir[i].z * delta;
			if (test == 0.0) {
				vec[i] = dir[i];
				infpt[i] = 1;
			}
			else {
				float lambda = delta / dir[i].z;
				vec[i] = anchor + (lambda * dir[i]);
				if(test < 0.0)
					infpt[i] = 2;
			}
		}

		for (i=0; i<4; i++)
		{
			// store the finite intersection points
			if (infpt[i] == 0)
				res.push_back(Vector4(vec[i].x, vec[i].y, vec[i].z, 1.0));
			else
			{
				// handle the infinite points of intersection;
				// cases split up into the possible frustum planes 
				// pieces which may contain a finite intersection point
				int nextind = (i+1) % 4;
				int prevind = (i+3) % 4;
				if ((infpt[prevind] == 0) || (infpt[nextind] == 0))
				{
					if (infpt[i] == 1)
						res.push_back(Vector4(vec[i].x, vec[i].y, vec[i].z, 0.0));
					else
					{
						// handle the intersection points that straddle infinity (back-project)
						if(infpt[prevind] == 0) 
						{
							Vector3 temp = vec[prevind] - vec[i];
							res.push_back(Vector4(temp.x, temp.y, temp.z, 0.0));
						}
						if(infpt[nextind] == 0)
						{
							Vector3 temp = vec[nextind] - vec[i];
							res.push_back(Vector4(temp.x, temp.y, temp.z, 0.0));
						}
					}
				} // end if we need to add an intersection point to the list
			} // end if infinite point needs to be considered
		} // end loop over frustun corners

		// we end up with either 0, 3, 4, or 5 intersection points

		return res;
	}

	//_______________________________________________________
	//|														|
	//|	forwardIntersect									|
	//|	-----------------------------						|
	//|	Forward intersect the camera's frustum rays with	|
	//| a specified plane of interest.						|
	//| Note that if the frustum rays shoot out and would	|
	//| back project onto the plane, this means the forward	|
	//| intersection of the frustum would occur at the		|
	//| line at infinity.									|
	//|_____________________________________________________|
	//
	void Camera::forwardIntersect(const Plane& worldPlane, vector<Vector4>::type* intersect3d) const
	{
		if(!intersect3d)
			return;

		Vector3 trCorner = getWorldSpaceCorners()[0];
		Vector3 tlCorner = getWorldSpaceCorners()[1];
		Vector3 blCorner = getWorldSpaceCorners()[2];
		Vector3 brCorner = getWorldSpaceCorners()[3];

		// need some sort of rotation that will bring the plane normal to the z axis
		Plane pval = worldPlane;
		if(pval.normal.z < 0.0)
		{
			pval.normal *= -1.0;
			pval.d *= -1.0;
		}
		Quaternion invPlaneRot = pval.normal.getRotationTo(Vector3::UNIT_Z);

		// get rotated light
		Vector3 lPos = invPlaneRot * getDerivedPosition();
		Vector3 vec[4];
		vec[0] = invPlaneRot * trCorner - lPos;
		vec[1] = invPlaneRot * tlCorner - lPos; 
		vec[2] = invPlaneRot * blCorner - lPos; 
		vec[3] = invPlaneRot * brCorner - lPos; 

		// compute intersection points on plane
		vector<Vector4>::type iPnt = getRayForwardIntersect(lPos, vec, -pval.d);


		// return wanted data
		if(intersect3d) 
		{
			Quaternion planeRot = invPlaneRot.Inverse();
			(*intersect3d).clear();
			for(unsigned int i=0; i<iPnt.size(); i++)
			{
				Vector3 intersection = planeRot * Vector3(iPnt[i].x, iPnt[i].y, iPnt[i].z);
				(*intersect3d).push_back(Vector4(intersection.x, intersection.y, intersection.z, iPnt[i].w));
			}
		}
	}
	//-----------------------------------------------------------------------
	void Camera::synchroniseBaseSettingsWith(const Camera* cam)
	{
		this->setPosition(cam->getPosition());
		this->setProjectionType(cam->getProjectionType());
		this->setOrientation(cam->getOrientation());
		this->setAspectRatio(cam->getAspectRatio());
		this->setNearClipDistance(cam->getNearClipDistance());
		this->setFarClipDistance(cam->getFarClipDistance());
		this->setFOVy(cam->getFOVy());
		this->setFocalLength(cam->getFocalLength());

		// Don't do these, they're not base settings and can cause referencing issues
		//this->setLodCamera(cam->getLodCamera());
		//this->setCullingFrustum(cam->getCullingFrustum());

	}


} // namespace CamelotEngine

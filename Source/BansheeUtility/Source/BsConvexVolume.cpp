//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsConvexVolume.h"
#include "BsAABox.h"
#include "BsSphere.h"
#include "BsPlane.h"
#include "BsMath.h"

namespace bs
{
	/**	Clip planes that form the camera frustum (visible area). */
	enum FrustumPlane
	{
		FRUSTUM_PLANE_NEAR = 0,
		FRUSTUM_PLANE_FAR = 1,
		FRUSTUM_PLANE_LEFT = 2,
		FRUSTUM_PLANE_RIGHT = 3,
		FRUSTUM_PLANE_TOP = 4,
		FRUSTUM_PLANE_BOTTOM = 5
	};

	ConvexVolume::ConvexVolume(const Vector<Plane>& planes)
		:mPlanes(planes)
	{ }

	ConvexVolume::ConvexVolume(const Matrix4& projectionMatrix)
	{
		mPlanes.resize(6);

		const Matrix4& proj = projectionMatrix;

		mPlanes[FRUSTUM_PLANE_LEFT].normal.x = proj[3][0] + proj[0][0];
		mPlanes[FRUSTUM_PLANE_LEFT].normal.y = proj[3][1] + proj[0][1];
		mPlanes[FRUSTUM_PLANE_LEFT].normal.z = proj[3][2] + proj[0][2];
		mPlanes[FRUSTUM_PLANE_LEFT].d = proj[3][3] + proj[0][3];

		mPlanes[FRUSTUM_PLANE_RIGHT].normal.x = proj[3][0] - proj[0][0];
		mPlanes[FRUSTUM_PLANE_RIGHT].normal.y = proj[3][1] - proj[0][1];
		mPlanes[FRUSTUM_PLANE_RIGHT].normal.z = proj[3][2] - proj[0][2];
		mPlanes[FRUSTUM_PLANE_RIGHT].d = proj[3][3] - proj[0][3];

		mPlanes[FRUSTUM_PLANE_TOP].normal.x = proj[3][0] - proj[1][0];
		mPlanes[FRUSTUM_PLANE_TOP].normal.y = proj[3][1] - proj[1][1];
		mPlanes[FRUSTUM_PLANE_TOP].normal.z = proj[3][2] - proj[1][2];
		mPlanes[FRUSTUM_PLANE_TOP].d = proj[3][3] - proj[1][3];

		mPlanes[FRUSTUM_PLANE_BOTTOM].normal.x = proj[3][0] + proj[1][0];
		mPlanes[FRUSTUM_PLANE_BOTTOM].normal.y = proj[3][1] + proj[1][1];
		mPlanes[FRUSTUM_PLANE_BOTTOM].normal.z = proj[3][2] + proj[1][2];
		mPlanes[FRUSTUM_PLANE_BOTTOM].d = proj[3][3] + proj[1][3];

		mPlanes[FRUSTUM_PLANE_NEAR].normal.x = proj[3][0] + proj[2][0];
		mPlanes[FRUSTUM_PLANE_NEAR].normal.y = proj[3][1] + proj[2][1];
		mPlanes[FRUSTUM_PLANE_NEAR].normal.z = proj[3][2] + proj[2][2];
		mPlanes[FRUSTUM_PLANE_NEAR].d = proj[3][3] + proj[2][3];

		mPlanes[FRUSTUM_PLANE_FAR].normal.x = proj[3][0] - proj[2][0];
		mPlanes[FRUSTUM_PLANE_FAR].normal.y = proj[3][1] - proj[2][1];
		mPlanes[FRUSTUM_PLANE_FAR].normal.z = proj[3][2] - proj[2][2];
		mPlanes[FRUSTUM_PLANE_FAR].d = proj[3][3] - proj[2][3];

		for (UINT32 i = 0; i < 6; i++)
		{
			float length = mPlanes[i].normal.normalize();
			mPlanes[i].d /= -length;
		}
	}

	bool ConvexVolume::intersects(const AABox& box) const
	{
		Vector3 center = box.getCenter();
		Vector3 extents = box.getHalfSize();
		Vector3 absExtents(Math::abs(extents.x), Math::abs(extents.y), Math::abs(extents.z));

		for (auto& plane : mPlanes)
		{
			float dist = center.dot(plane.normal) - plane.d;

			float effectiveRadius = absExtents.x * Math::abs(plane.normal.x);
			effectiveRadius += absExtents.y * Math::abs(plane.normal.y);
			effectiveRadius += absExtents.z * Math::abs(plane.normal.z);

			if (dist < -effectiveRadius)
				return false;
		}

		return true;
	}

	bool ConvexVolume::intersects(const Sphere& sphere) const
	{
		Vector3 center = sphere.getCenter();
		float radius = sphere.getRadius();

		for (auto& plane : mPlanes)
		{
			float dist = center.dot(plane.normal) - plane.d;

			if (dist < -radius)
				return false;
		}

		return true;
	}
}
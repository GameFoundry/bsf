#include "BsConvexVolume.h"
#include "BsAABox.h"
#include "BsSphere.h"
#include "BsPlane.h"

namespace BansheeEngine
{
	ConvexVolume::ConvexVolume(const Vector<Plane>& planes)
		:mPlanes(planes)
	{ }

	bool ConvexVolume::intersects(const AABox& box) const
	{
		Vector3 center = box.getCenter();
		Vector3 extents = box.getHalfSize();
		Vector3 absExtents(Math::abs(extents.x), Math::abs(extents.y), Math::abs(extents.z));

		for (auto& plane : mPlanes)
		{
			float dist = center.x * plane.normal.x;
			dist += center.y * plane.normal.y;
			dist += center.z * plane.normal.z;
			dist = dist - plane.d;

			float pushOut = absExtents.x * Math::abs(plane.normal.x);
			pushOut += absExtents.y * Math::abs(plane.normal.y);
			pushOut += absExtents.z * Math::abs(plane.normal.z);

			if (dist > pushOut)
				return false;
		}
	}

	bool ConvexVolume::intersects(const Sphere& sphere) const
	{
		Vector3 center = sphere.getCenter();
		float radius = sphere.getRadius();

		for (auto& plane : mPlanes)
		{
			float dist = center.x * plane.normal.x;
			dist += center.y * plane.normal.y;
			dist += center.z * plane.normal.z;
			dist = dist - plane.d;

			if (dist > radius)
				return false;
		}

		return true;
	}
}
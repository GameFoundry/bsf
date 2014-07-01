#pragma once

#include "BsPrerequisitesUtil.h"
#include "BsPlane.h"

namespace BansheeEngine
{
	/**
	 * @brief	Represents a convex volume defined by planes representing
	 *			the volume border.
	 */
	class BS_UTILITY_EXPORT ConvexVolume
	{
	public:
		ConvexVolume(const Vector<Plane>& planes);

		/**
		 * @brief	Checks does the volume intersects the provided axis aligned box.
		 *			This will return true if the box is fully inside the volume.
		 */
		bool intersects(const AABox& box) const;

		/**
		 * @brief	Checks does the volume intersects the provided sphere.
		 *			This will return true if the sphere is fully inside the volume.
		 */
		bool intersects(const Sphere& sphere) const;

	private:
		Vector<Plane> mPlanes;
	};
}
#pragma once

#include "BsPrerequisitesUtil.h"
#include "BsVector3.h"

namespace BansheeEngine
{
	/**
	 * @brief	Represents a rectangle in three dimensional space. It is represented
	 *			by two axes that extend from the specified origin. Axes should be perpendicular
	 *			to each other and they extend in both positive and negative directions from the
	 *			origin by the amount specified by extents.
	 */
	class BS_UTILITY_EXPORT Rect3
	{
	public:
		Rect3();

		Rect3(const Vector3& center, const std::array<Vector3, 2>& axes,
			const std::array<float, 2>& extents);

		/**
		 * @brief	Find the nearest points of the provided ray and the rectangle.
		 *
		 * @return	A set of nearest points and nearest distance.
		 *			First value in the set corresponds to nearest point on the ray, and the second to the nearest point on the rectangle.
		 *			They are same in the case of intersection. When ray is parallel to the rectangle there are two sets of nearest points
		 *			but only one the set nearest to the ray origin is returned.
		 */
		std::pair<std::array<Vector3, 2>, float> getNearestPoint(const Ray& ray) const;

		/**
		 * @brief	Find the nearest point on the rectangle to the provided point.
		 *
		 * @return	Nearest point and distance to nearest point.
		 */
		std::pair<Vector3, float> getNearestPoint(const Vector3& point) const;

		/**
		 * @brief	Ray/rectangle intersection.
		 *
		 * @return	Boolean result and distance to intersection point.
		 */
		std::pair<bool, float> intersects(const Ray& ray) const;

		/**
		 * @brief	Gets the origin of the rectangle. 
		 */
		const Vector3& getCenter() const { return mCenter; }

		/**
		 * @brief	Returns the rectangles horizontal axis.
		 */
		const Vector3& getAxisHorz() const { return mAxes[0]; }

		/**
		 * @brief	Returns the rectangles vertical axis.
		 */
		const Vector3& getAxisVert() const { return mAxes[1]; }

		/**
		 * @brief	Gets the extent of the rectangle along its horizontal axis.
		 */
		const float& getExtentHorz() const { return mExtents[0]; }

		/**
		 * @brief	Gets the extent of the rectangle along its vertical axis.
		 */
		const float& getExtentVertical() const { return mExtents[1]; }
	private:
		Vector3 mCenter;
		std::array<Vector3, 2> mAxes;
		std::array<float, 2> mExtents;
	};
}
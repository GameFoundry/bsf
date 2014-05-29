#pragma once

#include "BsPrerequisitesUtil.h"

#include "BsVector3.h"
#include "BsMatrix4.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Axis aligned box represented by minimum and maximum point.
	 */
	class BS_UTILITY_EXPORT AABox
	{
	public:
		/*
		   1-----2
		  /|    /|
		 / |   / |
		5-----4  |
		|  0--|--3
		| /   | /
		|/    |/
		6-----7
		*/
		typedef enum CornerEnum {
			FAR_LEFT_BOTTOM = 0,
			FAR_LEFT_TOP = 1,
			FAR_RIGHT_TOP = 2,
			FAR_RIGHT_BOTTOM = 3,
			NEAR_RIGHT_BOTTOM = 7,
			NEAR_LEFT_BOTTOM = 6,
			NEAR_LEFT_TOP = 5,
			NEAR_RIGHT_TOP = 4
		};

		AABox();
		AABox(const AABox& copy);
		AABox(const Vector3& min, const Vector3& max);

		AABox& operator=(const AABox& rhs);

		~AABox() { }

		const Vector3& getMin() const { return mMinimum; }
		const Vector3& getMax() const { return mMaximum; }

		void setMin(const Vector3& vec) { mMinimum = vec; }
		void setMax(const Vector3& vec) { mMaximum = vec; }

		void setExtents(const Vector3& min, const Vector3& max);
		void scale(const Vector3& s);

		Vector3 getCorner(CornerEnum cornerToGet) const;

		/**
		 * @brief	Merges the two boxes, creating a new 
		 * 			bounding box that encapsulates them both.
		 */
		void merge(const AABox& rhs);

		/**
		 * @brief	Expands the bounding box so it includes
		 * 			the provided point.
		 */
		void merge(const Vector3& point);

		/**
		 * @brief	Transforms the bounding box by the given matrix.
		 *
		 * @note	As the resulting box will no longer be axis aligned, an axis align box 
		 * 			is instead created by encompassing the transformed oriented bounding box.
		 * 			Retrieving the value as an actual OBB would provide a tighter fit.
		 */
		void transform(const Matrix4& matrix);

		/**
		 * @brief	Transforms the bounding box by the given matrix.
		 *
		 * @note	As the resulting box will no longer be axis aligned, an axis align box 
		 * 			is instead created by encompassing the transformed oriented bounding box.
		 * 			Retrieving the value as an actual OBB would provide a tighter fit.
		 * 			
		 *			Provided matrix must be affine.
		 */
		void transformAffine(const Matrix4& matrix);

		/**
		 * @brief	Returns true if this and the provided box intersect.
		 */
		bool intersects(const AABox& b2) const;

		/**
		 * @brief	Returns true if the sphere intersects the bounding box.
		 */
		bool intersects(const Sphere& s) const;

		/**
		 * @brief	Returns true if the plane intersects the bounding box.
		 */
		bool intersects(const Plane& p) const;

        /**
         * @brief	Ray / box intersection, returns a boolean result and nearest distance
         * 			to intersection.
         */
        std::pair<bool, float> intersects(const Ray& ray) const;

        /**
         * @brief	Ray / box intersection, returns boolean result and near and far intersection distance.
         */
        bool intersects(const Ray& ray, float& d1, float& d2) const;

		Vector3 getCenter() const;
		Vector3 getSize() const;
		Vector3 getHalfSize() const;
		float getRadius() const;
		float getVolume() const;

        /**
         * @brief	Returns true if the provided point is inside the bounding box.
         */
        bool contains(const Vector3& v) const;

        /**
         * @brief	Returns true if the provided bounding box is completely inside the bounding box.
         */
        bool contains(const AABox& other) const;

        bool operator== (const AABox& rhs) const;
        bool operator!= (const AABox& rhs) const;

		static const AABox BOX_EMPTY;

	protected:
		Vector3 mMinimum;
		Vector3 mMaximum;
	};
}

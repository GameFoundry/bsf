#pragma once

#include "CmPrerequisitesUtil.h"

#include "CmVector3.h"
#include "CmMatrix4.h"

namespace CamelotFramework 
{
	class CM_UTILITY_EXPORT AABox
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

		void merge(const AABox& rhs);
		void merge(const Vector3& point);

		void transform(const Matrix4& matrix);
		void transformAffine(const Matrix4& matrix);

		AABox intersection(const AABox& b2) const;

		bool intersects(const AABox& b2) const;
		bool intersects(const Sphere& s) const;
		bool intersects(const Plane& p) const;
		bool intersects(const Vector3& v) const;

        /**
         * @brief	Ray / box intersection, returns boolean result and distance.
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

        bool contains(const Vector3& v) const;
        bool contains(const AABox& other) const;

        bool operator== (const AABox& rhs) const;
        bool operator!= (const AABox& rhs) const;

		static const AABox BOX_EMPTY;

	protected:
		Vector3 mMinimum;
		Vector3 mMaximum;
	};
}

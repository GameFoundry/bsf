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
		AABox(const AABox & rkBox);
		AABox(const Vector3& min, const Vector3& max);

		AABox& operator=(const AABox& rhs);

		~AABox() { }

		const Vector3& getMin(void) const { return mMinimum; }
		const Vector3& getMax(void) const { return mMaximum; }

		void setMin(const Vector3& vec) { mMinimum = vec; }
		void setMax(const Vector3& vec) { mMaximum = vec; }

		void setExtents(const Vector3& min, const Vector3& max);

		Vector3 getCorner(CornerEnum cornerToGet) const;

		void merge(const AABox& rhs);
		void merge(const Vector3& point);

		void transform(const Matrix4& matrix);
		void transformAffine(const Matrix4& matrix);

		bool intersects(const AABox& b2) const;

		AABox intersection(const AABox& b2) const;

		float volume() const;

		inline void scale(const Vector3& s);

		bool intersects(const Sphere& s) const;
		bool intersects(const Plane& p) const;
		bool intersects(const Vector3& v) const;

		Vector3 getCenter() const;
		Vector3 getSize() const;
		Vector3 getHalfSize() const;

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

#pragma once

#include "BsEditorPrerequisites.h"
#include "BsHandleSlider.h"
#include "BsCapsule.h"
#include "BsSphere.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT HandleSliderLine : public HandleSlider
	{
	public:
		HandleSliderLine(const Vector3& direction, float length, float snapValue, bool fixedScale);
		~HandleSliderLine();

		bool intersects(const Ray& ray, float& t) const;
		void update(const HCamera& camera, const Vector2I& pointerPos, const Ray& ray);

		float getDelta() const { return mDelta; }
		Vector3 getNewPosition() const;

	protected:
		void reset();

		static const float CAPSULE_RADIUS;
		static const float SPHERE_RADIUS;

		Vector3 mDirection;
		float mLength;

		Capsule mCapsuleCollider;
		Sphere mSphereCollider;

		float mDelta;
		bool mHasLastPos;
	};
}
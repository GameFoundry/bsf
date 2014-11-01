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
		HandleSliderLine(const Vector3& direction, float length, bool fixedScale);
		~HandleSliderLine();

		bool intersects(const Ray& ray, float& t) const;
		void handleInput(const CameraHandlerPtr& camera, const Vector2I& pointerPos, const Ray& ray);

		Vector3 getNewPosition() const;

	protected:

		static const float CAPSULE_RADIUS;
		static const float SPHERE_RADIUS;

		Vector3 mDirection;
		float mLength;

		Capsule mCapsuleCollider;
		Sphere mSphereCollider;
	};
}
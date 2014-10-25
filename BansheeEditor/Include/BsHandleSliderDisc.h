#pragma once

#include "BsEditorPrerequisites.h"
#include "BsHandleSlider.h"
#include "BsTorus.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT HandleSliderDisc : public HandleSlider
	{
	public:
		HandleSliderDisc(const Vector3& normal, float radius, bool fixedScale);
		~HandleSliderDisc();

		bool intersects(const Ray& ray, float& t) const;
		void handleInput(const HCamera& camera, const Vector2I& pointerPos, const Ray& ray);

		Vector3 getDeltaDirection() const { return Vector3::ZERO; /* TODO */ }
		Quaternion getNewRotation() const { return mRotation; /* TODO */ }

	protected:
		virtual void updateCachedTransform() const;

		static const float TORUS_RADIUS;

		Vector3 mNormal;
		float mRadius;
		Matrix4 mTorusRotation;

		Torus mCollider;
	};
}
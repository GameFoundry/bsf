#pragma once

#include "BsEditorPrerequisites.h"
#include "BsHandleSlider.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT HandleSliderDisc : public HandleSlider
	{
	public:
		HandleSliderDisc(const Vector3& normal, float radius, float snapValue, bool fixedScale);
		~HandleSliderDisc();

		Quaternion updateDelta(const Quaternion& oldValue) const;

		float getDelta() const { return 0.0f; /* TODO */ }
		Vector3 getDeltaDirection() const { return Vector3::ZERO; /* TODO */ }
		Quaternion getNewRotation() const { return mRotation; /* TODO */ }

		virtual const Matrix4& getTransform() const;
	protected:
		static const float TORUS_RADIUS;

		Vector3 mNormal;
		float mRadius;
		Matrix4 mTorusRotation;
	};
}
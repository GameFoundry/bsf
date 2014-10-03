#pragma once

#include "BsEditorPrerequisites.h"
#include "BsHandleSlider.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT HandleSliderPlane : public HandleSlider
	{
	public:
		HandleSliderPlane(const Vector3& dir1, const Vector3& dir2, float length, float snapValue, bool fixedScale);
		~HandleSliderPlane();

		Vector3 updateDelta(const Vector3& oldValue) const;

		float getDelta() const { return 0.0f; /* TODO */ }
		Vector3 getDeltaDirection() const { return Vector3::ZERO; /* TODO */ }
		Vector3 getNewPosition() const { return mPosition; /* TODO */ }

	protected:
		Vector3 mDirection1;
		Vector3 mDirection2;
		float mLength;
	};
}
#pragma once

#include "BsEditorPrerequisites.h"
#include "BsHandleSlider.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT HandleSliderLine : public HandleSlider
	{
	public:
		HandleSliderLine(const Vector3& direction, float length, float snapValue, bool fixedScale);
		~HandleSliderLine();

		Vector3 updateDelta(const Vector3& oldValue) const;

	protected:
		static const float CAPSULE_RADIUS;
		static const float SPHERE_RADIUS;

		Vector3 mDirection;
		float mLength;
	};
}
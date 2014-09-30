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

		void setCustomTransform(const Matrix4& transform);
	protected:
		static const float TORUS_RADIUS;

		Vector3 mNormal;
		float mRadius;
		Matrix4 mTorusRotation;
	};
}
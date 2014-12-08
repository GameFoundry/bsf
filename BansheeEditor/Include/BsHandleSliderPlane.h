#pragma once

#include "BsEditorPrerequisites.h"
#include "BsHandleSlider.h"
#include "BsRect3.h"
#include "BsVector2.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT HandleSliderPlane : public HandleSlider
	{
	public:
		HandleSliderPlane(const Vector3& dir1, const Vector3& dir2, float length, bool fixedScale);
		~HandleSliderPlane();

		bool intersects(const Ray& ray, float& t) const;
		void handleInput(const CameraHandlerPtr& camera, const Vector2I& inputDelta);

		Vector2 getDelta() const { return mDelta; }
	protected:
		void activate(const CameraHandlerPtr& camera, const Vector2I& pointerPos) { mStartPosition = getPosition(); }
		void reset() { mDelta = Vector2::ZERO; }

		Vector3 mDirection1;
		Vector3 mDirection2;
		float mLength;

		Rect3 mCollider;

		Vector2 mDelta;
		Vector3 mStartPosition;
	};
}
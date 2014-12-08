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
		void handleInput(const CameraHandlerPtr& camera, const Vector2I& inputDelta);

		float getDelta() const { return mDelta; }

	protected:
		void activate(const CameraHandlerPtr& camera, const Vector2I& pointerPos);
		void reset() { mDelta = 0.0f; }
		virtual void updateCachedTransform() const;

		Vector3 calculateClosestPointOnArc(const Ray& inputRay, const Vector3& center, const Vector3& up,
			float radius, Degree startAngle, Degree angleAmount);

		Degree pointOnCircleToAngle(Vector3 up, Vector3 point);

		static const float TORUS_RADIUS;

		Vector3 mNormal;
		float mRadius;
		Matrix4 mTorusRotation;

		Vector3 mDirection;
		Vector3 mStartPosition;
		float mDelta;

		Torus mCollider;
	};
}
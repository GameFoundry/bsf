#pragma once

#include "BsPrerequisites.h"
#include "CmComponent.h"
#include "CmMath.h"

namespace CamelotFramework
{
	class DebugCamera : public Component
	{
	private:
		float mCurrentSpeed;

		Degree mPitch;
		Degree mYaw;
		bool mLastButtonState;

		BS::HCamera mCamera;

		static const float START_SPEED;
		static const float TOP_SPEED;
		static const float ACCELERATION;
		static const float FAST_MODE_MULTIPLIER;
		static const float ROTATION_SPEED; // Degrees/second

		/************************************************************************/
		/* 						COMPONENT OVERRIDES                      		*/
		/************************************************************************/
	protected:
		friend class SceneObject;

		/** Standard constructor.
        */
		DebugCamera(const HSceneObject& parent);

	public:
		virtual void update();
	};
}
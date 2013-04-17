#pragma once

#include "BsPrerequisites.h"
#include "CmComponent.h"
#include "CmInputHandler.h"
#include "CmMath.h"

namespace CamelotFramework
{
	class DebugCamera : public Component
	{
	private:
		float mCurrentSpeed;

		bool mGoingForward;
		bool mGoingBack;
		bool mGoingLeft;
		bool mGoingRight;
		bool mFastMove;
		bool mCameraRotating;

		Degree mPitch;
		Degree mYaw;

		BS::HCamera mCamera;

		void keyDown(CamelotFramework::KeyCode keyCode);
		void keyUp(CamelotFramework::KeyCode keyCode);

		void mouseDown(const CamelotFramework::MouseEvent& event, CamelotFramework::MouseButton buttonID);
		void mouseUp(const CamelotFramework::MouseEvent& event, CamelotFramework::MouseButton buttonID);

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
#pragma once

#include "CmPrerequisites.h"
#include "CmComponent.h"
#include "CmInputHandler.h"
#include "CmMath.h"

namespace CamelotEngine
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

		HCamera mCamera;

		void keyDown(CamelotEngine::KeyCode keyCode);
		void keyUp(CamelotEngine::KeyCode keyCode);

		void mouseDown(const CamelotEngine::MouseEvent& event, CamelotEngine::MouseButton buttonID);
		void mouseUp(const CamelotEngine::MouseEvent& event, CamelotEngine::MouseButton buttonID);

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
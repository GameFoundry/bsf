//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPrerequisites.h"
#include "BsComponent.h"
#include "BsMath.h"
#include "BsVirtualInput.h"

namespace BansheeEngine
{
	class SceneCameraController : public Component
	{
	public:
		virtual void update();

		static const String MOVE_FORWARD_BTN;
		static const String MOVE_LEFT_BTN;
		static const String MOVE_RIGHT_BTN;
		static const String MOVE_BACKWARD_BTN;
		static const String FAST_MOVE_BTN;
		static const String ROTATE_BTN;
		static const String HORIZONTAL_AXIS;
		static const String VERTICAL_AXIS;

	private:
		float mCurrentSpeed;

		Degree mPitch;
		Degree mYaw;
		bool mLastButtonState;

		VirtualButton mMoveForward;
		VirtualButton mMoveLeft;
		VirtualButton mMoveRight;
		VirtualButton mMoveBackward;
		VirtualButton mFastMove;
		VirtualButton mRotate;
		VirtualAxis mHorizontal;
		VirtualAxis mVertical;

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

		SceneCameraController(const HSceneObject& parent);
	};
}
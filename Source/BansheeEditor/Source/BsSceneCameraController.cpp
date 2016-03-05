//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsSceneCameraController.h"
#include "BsVirtualInput.h"
#include "BsVector3.h"
#include "BsTime.h"
#include "BsMath.h"
#include "BsSceneObject.h"
#include "BsCamera.h"
#include "BsPlatform.h"
#include "BsEditorApplication.h"
#include "BsCursor.h"

#include "BsDebug.h"

namespace BansheeEngine
{
	const String SceneCameraController::MOVE_FORWARD_BTN = "SceneForward";
	const String SceneCameraController::MOVE_LEFT_BTN = "SceneLeft";
	const String SceneCameraController::MOVE_RIGHT_BTN = "SceneRight";
	const String SceneCameraController::MOVE_BACKWARD_BTN = "SceneBackward";
	const String SceneCameraController::FAST_MOVE_BTN = "SceneFastMove";
	const String SceneCameraController::ROTATE_BTN = "SceneRotate";
	const String SceneCameraController::HORIZONTAL_AXIS = "SceneHorizontal";
	const String SceneCameraController::VERTICAL_AXIS = "SceneVertical";

	const float SceneCameraController::START_SPEED = 4.0f;
	const float SceneCameraController::TOP_SPEED = 12.0f;
	const float SceneCameraController::ACCELERATION = 1.0f;
	const float SceneCameraController::FAST_MODE_MULTIPLIER = 2.0f;
	const float SceneCameraController::ROTATION_SPEED = 360.0f; // Degrees/second

	Degree wrapAngle(Degree angle)
	{
		if (angle.valueDegrees() < -360.0f)
			angle += Degree(360.0f);

		if (angle.valueDegrees() > 360.0f)
			angle -= Degree(360.0f);

		return angle;
	}

	SceneCameraController::SceneCameraController(const HSceneObject& parent)
		:Component(parent), mPitch(0.0f), mYaw(0.0f), mLastButtonState(false)
	{
		setName("SceneCameraController");

		mMoveForward = VirtualButton(MOVE_FORWARD_BTN);
		mMoveLeft = VirtualButton(MOVE_LEFT_BTN);
		mMoveRight = VirtualButton(MOVE_RIGHT_BTN);
		mMoveBackward = VirtualButton(MOVE_BACKWARD_BTN);
		mFastMove = VirtualButton(FAST_MOVE_BTN);
		mRotate = VirtualButton(ROTATE_BTN);
		mHorizontal = VirtualAxis(HORIZONTAL_AXIS);
		mVertical = VirtualAxis(VERTICAL_AXIS);
	}

	void SceneCameraController::update()
	{
		if (!gEditorApplication().isSceneViewFocused())
			return;

		bool goingForward = gVirtualInput().isButtonHeld(mMoveForward);
		bool goingBack = gVirtualInput().isButtonHeld(mMoveBackward);
		bool goingLeft = gVirtualInput().isButtonHeld(mMoveLeft);
		bool goingRight = gVirtualInput().isButtonHeld(mMoveRight);
		bool fastMove = gVirtualInput().isButtonHeld(mFastMove);
		bool camRotating = gVirtualInput().isButtonHeld(mRotate);

		if (camRotating != mLastButtonState)
		{
			if (camRotating)
				Cursor::instance().hide();
			else
				Cursor::instance().show();

			mLastButtonState = camRotating;
		}

		float frameDelta = gTime().getFrameDelta();
		if (camRotating)
		{
			mYaw += Degree(gVirtualInput().getAxisValue(mHorizontal) * ROTATION_SPEED * frameDelta);
			mPitch += Degree(gVirtualInput().getAxisValue(mVertical) * ROTATION_SPEED * frameDelta);

			mYaw = wrapAngle(mYaw);
			mPitch = wrapAngle(mPitch);

			Quaternion yRot;
			yRot.fromAxisAngle(Vector3::UNIT_Y, Radian(mYaw));

			Quaternion xRot;
			xRot.fromAxisAngle(Vector3::UNIT_X, Radian(mPitch));

			Quaternion camRot = yRot * xRot;
			camRot.normalize();

			SO()->setRotation(camRot);
		}

		Vector3 direction = Vector3::ZERO;
		if (goingForward) direction += SO()->getForward();
		if (goingBack) direction -= SO()->getForward();
		if (goingRight) direction += SO()->getRight();
		if (goingLeft) direction -= SO()->getRight();

		if (direction.squaredLength() != 0)
		{
			direction.normalize();

			float multiplier = 1.0f;
			if (fastMove)
				multiplier = FAST_MODE_MULTIPLIER;

			mCurrentSpeed = Math::clamp(mCurrentSpeed + ACCELERATION * frameDelta, START_SPEED, TOP_SPEED);
			mCurrentSpeed *= multiplier;
		}
		else
		{
			mCurrentSpeed = 0.0f;
		}

		float tooSmall = std::numeric_limits<float>::epsilon();
		if (mCurrentSpeed > tooSmall)
		{
			Vector3 velocity = direction * mCurrentSpeed;
			SO()->move(velocity * frameDelta);
		}
	}
}
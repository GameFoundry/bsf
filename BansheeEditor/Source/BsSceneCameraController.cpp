#include "BsSceneCameraController.h"
#include "BsVirtualInput.h"
#include "BsVector3.h"
#include "BsTime.h"
#include "BsMath.h"
#include "BsSceneObject.h"
#include "BsCamera.h"
#include "BsPlatform.h"
#include "BsEditorApplication.h"

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

	const float SceneCameraController::START_SPEED = 40.0f;
	const float SceneCameraController::TOP_SPEED = 130.0f;
	const float SceneCameraController::ACCELERATION = 10.0f;
	const float SceneCameraController::FAST_MODE_MULTIPLIER = 2.0f;
	const float SceneCameraController::ROTATION_SPEED = 0.5f; // Degrees/pixel

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

		mCamera = sceneObject()->getComponent<Camera>();
		mCamera->setNearClipDistance(5);

		sceneObject()->setPosition(Vector3(0, 0, 0));
		sceneObject()->lookAt(Vector3(0, 0, -1));
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
				Platform::hideCursor();
			else
				Platform::showCursor();

			mLastButtonState = camRotating;
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

			mCurrentSpeed = Math::clamp(mCurrentSpeed + ACCELERATION * gTime().getFrameDelta(), START_SPEED, TOP_SPEED);
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
			SO()->move(velocity * gTime().getFrameDelta());
		}

		if (camRotating)
		{
			mYaw += Degree(gVirtualInput().getAxisValue(mHorizontal) * ROTATION_SPEED);
			mPitch += Degree(gVirtualInput().getAxisValue(mVertical) * ROTATION_SPEED);

			Quaternion yRot;
			yRot.fromAxisAngle(Vector3::UNIT_Y, Radian(mYaw));

			Quaternion xRot;
			xRot.fromAxisAngle(yRot.xAxis(), Radian(mPitch));

			Quaternion camRot = xRot * yRot;

			SO()->setRotation(camRot);
		}
	}
}
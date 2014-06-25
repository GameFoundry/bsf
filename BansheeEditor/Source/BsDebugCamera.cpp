#include "BsDebugCamera.h"
#include "BsInput.h"
#include "BsVector3.h"
#include "BsTime.h"
#include "BsMath.h"
#include "BsSceneObject.h"
#include "BsCamera.h"
#include "BsPlatform.h"

namespace BansheeEngine
{
	const float DebugCamera::START_SPEED = 40.0f;
	const float DebugCamera::TOP_SPEED = 130.0f;
	const float DebugCamera::ACCELERATION = 10.0f;
	const float DebugCamera::FAST_MODE_MULTIPLIER = 2.0f;
	const float DebugCamera::ROTATION_SPEED = 0.5f; // Degrees/pixel

	DebugCamera::DebugCamera(const HSceneObject& parent)
		:Component(parent), mPitch(0.0f), mYaw(0.0f), mLastButtonState(false)
	{
		setName("DebugCamera");

		mCamera = sceneObject()->getComponent<Camera>();
		mCamera->setNearClipDistance(5);

		sceneObject()->setPosition(Vector3(0,0,0));
		sceneObject()->lookAt(Vector3(0,0,-1));
	}

	void DebugCamera::update()
	{
		bool goingForward = gInput().isButtonHeld(BC_W) || gInput().isButtonHeld(BC_UP);
		bool goingBack = gInput().isButtonHeld(BC_S) || gInput().isButtonHeld(BC_DOWN);
		bool goingLeft = gInput().isButtonHeld(BC_A) || gInput().isButtonHeld(BC_LEFT);
		bool goingRight = gInput().isButtonHeld(BC_D) || gInput().isButtonHeld(BC_RIGHT);
		bool fastMove = gInput().isButtonHeld(BC_LSHIFT);
		bool camRotating = gInput().isButtonHeld(BC_MOUSE_RIGHT);

		if(camRotating != mLastButtonState)
		{
			if(camRotating)
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
			if(fastMove)
				multiplier = FAST_MODE_MULTIPLIER;

			mCurrentSpeed = Math::clamp(mCurrentSpeed + ACCELERATION * gTime().getFrameDelta(), START_SPEED, TOP_SPEED);
			mCurrentSpeed *= multiplier;
		}
		else
		{
			mCurrentSpeed = 0.0f;
		}

		float tooSmall = std::numeric_limits<float>::epsilon();
		if(mCurrentSpeed > tooSmall)
		{
			Vector3 velocity = direction * mCurrentSpeed;
			SO()->move(velocity * gTime().getFrameDelta());
		}

		if(camRotating)
		{
			mYaw += Degree(gInput().getAxisValue(InputDevice::Mouse, InputAxis::MainX) * ROTATION_SPEED);
			mPitch += Degree(gInput().getAxisValue(InputDevice::Mouse, InputAxis::MainY) * ROTATION_SPEED);

			Quaternion yRot;
			yRot.fromAxisAngle(Vector3::UNIT_Y, Radian(mYaw));

			Quaternion xRot;
			xRot.fromAxisAngle(yRot.xAxis(), Radian(mPitch));

			Quaternion camRot = xRot * yRot;

			SO()->setRotation(camRot);
		}
	}
}
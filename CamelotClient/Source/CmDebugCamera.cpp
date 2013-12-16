#include "CmDebugCamera.h"
#include "CmInput.h"
#include "CmVector3.h"
#include "CmTime.h"
#include "CmMath.h"
#include "CmSceneObject.h"
#include "BsCamera.h"
#include "CmPlatform.h"

using namespace BansheeEngine;

namespace CamelotFramework
{
	const float DebugCamera::START_SPEED = 40.0f;
	const float DebugCamera::TOP_SPEED = 130.0f;
	const float DebugCamera::ACCELERATION = 10.0f;
	const float DebugCamera::FAST_MODE_MULTIPLIER = 2.0f;
	const float DebugCamera::ROTATION_SPEED = 0.5f; // Degrees/pixel

	DebugCamera::DebugCamera(const HSceneObject& parent)
		:Component(parent), mPitch(0.0f), mYaw(0.0f), mLastButtonState(false)
	{
		mCamera = sceneObject()->getComponent<Camera>();
		mCamera->setNearClipDistance(5);

		sceneObject()->setPosition(Vector3(0,0,0));
		sceneObject()->lookAt(Vector3(0,0,-1));
	}

	void DebugCamera::update()
	{
		bool goingForward = gInput().isButtonDown(BC_W) || gInput().isButtonDown(BC_UP);
		bool goingBack = gInput().isButtonDown(BC_S) || gInput().isButtonDown(BC_DOWN);
		bool goingLeft = gInput().isButtonDown(BC_A) || gInput().isButtonDown(BC_LEFT);
		bool goingRight = gInput().isButtonDown(BC_D) || gInput().isButtonDown(BC_RIGHT);
		bool fastMove = gInput().isButtonDown(BC_LSHIFT);
		bool camRotating = gInput().isButtonDown(BC_MOUSE_RIGHT);

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
			mYaw += Degree(gInput().getHorizontalAxis() * ROTATION_SPEED);
			mPitch += Degree(gInput().getVerticalAxis() * ROTATION_SPEED);

			Quaternion yRot;
			yRot.fromAxisAngle(Vector3::UNIT_Y, Radian(mYaw));

			Quaternion xRot;
			xRot.fromAxisAngle(yRot.xAxis(), Radian(mPitch));

			Quaternion camRot = xRot * yRot;

			SO()->setRotation(camRot);
		}
	}
}
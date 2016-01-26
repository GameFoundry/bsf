#include "CameraFlyer.h"
#include "BsVector3.h"
#include "BsTime.h"
#include "BsMath.h"
#include "BsSceneObject.h"
#include "BsCamera.h"
#include "BsPlatform.h"
#include "BsCursor.h"
#include "BsDebug.h"

namespace BansheeEngine
{
	const float CameraFlyer::START_SPEED = 40.0f;
	const float CameraFlyer::TOP_SPEED = 130.0f;
	const float CameraFlyer::ACCELERATION = 10.0f;
	const float CameraFlyer::FAST_MODE_MULTIPLIER = 2.0f;
	const float CameraFlyer::ROTATION_SPEED = 360.0f; // Degrees/second

	Degree wrapAngle(Degree angle)
	{
		if (angle.valueDegrees() < -360.0f)
			angle += Degree(360.0f);

		if (angle.valueDegrees() > 360.0f)
			angle -= Degree(360.0f);

		return angle;
	}

	CameraFlyer::CameraFlyer(const HSceneObject& parent)
		:Component(parent), mPitch(0.0f), mYaw(0.0f), mLastButtonState(false)
	{
		setName("CameraFlyer");

		mCamera = sceneObject()->getComponent<Camera>();
		mCamera->setNearClipDistance(5);

		mMoveForward = VirtualButton("Forward");
		mMoveBack = VirtualButton("Back");
		mMoveLeft = VirtualButton("Left");
		mMoveRight = VirtualButton("Right");
		mFastMove = VirtualButton("FastMove");
		mRotateCam = VirtualButton("RotateCam");
		mHorizontalAxis = VirtualAxis("Horizontal");
		mVerticalAxis = VirtualAxis("Vertical");
	}

	void CameraFlyer::update()
	{
		bool goingForward = gVirtualInput().isButtonHeld(mMoveForward);
		bool goingBack = gVirtualInput().isButtonHeld(mMoveBack);
		bool goingLeft = gVirtualInput().isButtonHeld(mMoveLeft);
		bool goingRight = gVirtualInput().isButtonHeld(mMoveRight);
		bool fastMove = gVirtualInput().isButtonHeld(mFastMove);
		bool camRotating = gVirtualInput().isButtonHeld(mRotateCam);

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
			mYaw += Degree(gVirtualInput().getAxisValue(mHorizontalAxis) * ROTATION_SPEED * frameDelta);
			mPitch += Degree(gVirtualInput().getAxisValue(mVerticalAxis) * ROTATION_SPEED * frameDelta);

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
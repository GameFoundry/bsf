#include "CmDebugCamera.h"
#include "CmInput.h"
#include "CmVector3.h"
#include "CmTime.h"
#include "CmMath.h"
#include "CmGameObject.h"
#include "CmCamera.h"
#include "CmOSCursor.h"

namespace CamelotEngine
{
	const float DebugCamera::START_SPEED = 40.0f;
	const float DebugCamera::TOP_SPEED = 130.0f;
	const float DebugCamera::ACCELERATION = 10.0f;
	const float DebugCamera::FAST_MODE_MULTIPLIER = 2.0f;
	const float DebugCamera::ROTATION_SPEED = 0.5f; // Degrees/pixel

	DebugCamera::DebugCamera(const HGameObject& parent)
		:Component(parent), mGoingForward(false), mGoingBack(false), mGoingLeft(false), mGoingRight(false), 
		mFastMove(false), mCameraRotating(false), mPitch(0.0f), mYaw(0.0f)
	{
		mCamera = gameObject()->getComponent<Camera>();
		mCamera->setNearClipDistance(5);

		//gameObject()->setPosition(Vector3(0,0,5050));
		//gameObject()->lookAt(Vector3(0,0,-300));

		gameObject()->setPosition(Vector3(0,0,0));
		gameObject()->lookAt(Vector3(0,0,-1));

		gInput().onKeyDown.connect(boost::bind(&DebugCamera::keyDown, this, _1));
		gInput().onKeyUp.connect(boost::bind(&DebugCamera::keyUp, this, _1));

		gInput().onMouseDown.connect(boost::bind(&DebugCamera::mouseDown, this, _1, _2));
		gInput().onMouseUp.connect(boost::bind(&DebugCamera::mouseUp, this, _1, _2));
	}

	void DebugCamera::keyDown(KeyCode keyCode)
	{
		if (keyCode == KC_W || keyCode == KC_UP) 
			mGoingForward = true;
		else if (keyCode == KC_S || keyCode== KC_DOWN) 
			mGoingBack = true;
		else if (keyCode == KC_A || keyCode == KC_LEFT) 
			mGoingLeft = true;
		else if (keyCode == KC_D || keyCode == KC_RIGHT) 
			mGoingRight = true;
		else if (keyCode == KC_LSHIFT) 
			mFastMove = true;
	}

	void DebugCamera::keyUp(KeyCode keyCode)
	{
		if (keyCode == KC_W || keyCode == KC_UP) 
			mGoingForward = false;
		else if (keyCode == KC_S || keyCode== KC_DOWN) 
			mGoingBack = false;
		else if (keyCode == KC_A || keyCode == KC_LEFT) 
			mGoingLeft = false;
		else if (keyCode == KC_D || keyCode == KC_RIGHT) 
			mGoingRight = false;
		else if (keyCode == KC_LSHIFT) 
			mFastMove = false;
	}

	void DebugCamera::mouseDown(const MouseEvent& event, MouseButton buttonID)
	{
		if(buttonID == MB_Right)
		{
			mCameraRotating = true;

			OSCursor::hide();
		}
	}

	void DebugCamera::mouseUp(const MouseEvent& event, MouseButton buttonID)
	{
		if(buttonID == MB_Right)
		{
			mCameraRotating = false;

			OSCursor::show();
		}
	}

	void DebugCamera::update()
	{
		Vector3 direction = Vector3::ZERO;
		if (mGoingForward) direction += GO()->getForward();
		if (mGoingBack) direction -= GO()->getForward();
		if (mGoingRight) direction += GO()->getRight();
		if (mGoingLeft) direction -= GO()->getRight();

		if (direction.squaredLength() != 0)
		{
			direction.normalise();

			float multiplier = 1.0f;
			if(mFastMove)
				multiplier = FAST_MODE_MULTIPLIER;

			mCurrentSpeed = Math::Clamp(mCurrentSpeed + ACCELERATION * gTime().getFrameDelta(), START_SPEED, TOP_SPEED);
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
			GO()->move(velocity * gTime().getFrameDelta());
		}

		if(mCameraRotating)
		{
			mYaw += Degree(gInput().getHorizontalAxis() * ROTATION_SPEED);
			mPitch += Degree(gInput().getVerticalAxis() * ROTATION_SPEED);

			Quaternion yRot;
			yRot.FromAngleAxis(Radian(mYaw), Vector3::UP);

			Quaternion xRot;
			xRot.FromAngleAxis(Radian(mPitch), yRot.xAxis());

			Quaternion camRot = xRot * yRot;

			GO()->setRotation(camRot);
		}
	}
}
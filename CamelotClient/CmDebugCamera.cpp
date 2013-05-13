#include "CmDebugCamera.h"
#include "CmInput.h"
#include "CmVector3.h"
#include "CmTime.h"
#include "CmMath.h"
#include "CmSceneObject.h"
#include "BsCamera.h"
#include "CmCursor.h"

using namespace BansheeEngine;

namespace CamelotFramework
{
	const float DebugCamera::START_SPEED = 40.0f;
	const float DebugCamera::TOP_SPEED = 130.0f;
	const float DebugCamera::ACCELERATION = 10.0f;
	const float DebugCamera::FAST_MODE_MULTIPLIER = 2.0f;
	const float DebugCamera::ROTATION_SPEED = 0.5f; // Degrees/pixel

	DebugCamera::DebugCamera(const HSceneObject& parent)
		:Component(parent), mPitch(0.0f), mYaw(0.0f)
	{
		mCamera = sceneObject()->getComponent<Camera>();
		mCamera->setNearClipDistance(5);

		//gameObject()->setPosition(Vector3(0,0,5050));
		//gameObject()->lookAt(Vector3(0,0,-300));

		sceneObject()->setPosition(Vector3(0,0,0));
		sceneObject()->lookAt(Vector3(0,0,-1));
	}

	void DebugCamera::update()
	{
		bool goingForward = gInput().isKeyDown(KC_W) || gInput().isKeyDown(KC_UP);
		bool goingBack = gInput().isKeyDown(KC_S) || gInput().isKeyDown(KC_DOWN);
		bool goingLeft = gInput().isKeyDown(KC_A) || gInput().isKeyDown(KC_LEFT);
		bool goingRight = gInput().isKeyDown(KC_D) || gInput().isKeyDown(KC_RIGHT);
		bool fastMove = gInput().isKeyDown(KC_LSHIFT);
		bool camRotating = gInput().isButtonDown(MB_Right);

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
			SO()->move(velocity * gTime().getFrameDelta());
		}

		if(camRotating)
		{
			mYaw += Degree(gInput().getHorizontalAxis() * ROTATION_SPEED);
			mPitch += Degree(gInput().getVerticalAxis() * ROTATION_SPEED);

			Quaternion yRot;
			yRot.FromAngleAxis(Radian(mYaw), Vector3::UP);

			Quaternion xRot;
			xRot.FromAngleAxis(Radian(mPitch), yRot.xAxis());

			Quaternion camRot = xRot * yRot;

			SO()->setRotation(camRot);
		}
	}
}
//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPrerequisites.h"
#include "BsComponent.h"
#include "BsMath.h"
#include "BsVirtualInput.h"

namespace BansheeEngine
{
	class CameraFlyer : public Component
	{
	public:
		CameraFlyer(const HSceneObject& parent);

		virtual void update();

	private:
		float mCurrentSpeed;

		Degree mPitch;
		Degree mYaw;
		bool mLastButtonState;

		HCamera mCamera;

		VirtualButton mMoveForward;
		VirtualButton mMoveBack;
		VirtualButton mMoveLeft;
		VirtualButton mMoveRight;
		VirtualButton mFastMove;
		VirtualButton mRotateCam;
		VirtualAxis mVerticalAxis;
		VirtualAxis mHorizontalAxis;

		static const float START_SPEED;
		static const float TOP_SPEED;
		static const float ACCELERATION;
		static const float FAST_MODE_MULTIPLIER;
		static const float ROTATION_SPEED; // Degrees/second
	};
}
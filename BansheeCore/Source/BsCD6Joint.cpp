//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsCD6Joint.h"
#include "BsSceneObject.h"
#include "BsCD6JointRTTI.h"

namespace BansheeEngine
{
	CD6Joint::CD6Joint(const HSceneObject& parent)
		: CJoint(parent)
	{
		setName("D6Joint");
	}

	D6Joint::Motion CD6Joint::getMotion(D6Joint::Axis axis) const
	{
		return mMotion[(int)axis];
	}

	void CD6Joint::setMotion(D6Joint::Axis axis, D6Joint::Motion motion)
	{
		if (mMotion[(int)axis] == motion)
			return;

		mMotion[(int)axis] = motion;

		if (mInternal != nullptr)
			_getInternal()->setMotion(axis, motion);
	}

	Radian CD6Joint::getTwist() const
	{
		if (mInternal == nullptr)
			return Radian(0.0f);

		return _getInternal()->getTwist();
	}

	Radian CD6Joint::getSwingY() const
	{
		if (mInternal == nullptr)
			return Radian(0.0f);

		return _getInternal()->getSwingY();
	}

	Radian CD6Joint::getSwingZ() const
	{
		if (mInternal == nullptr)
			return Radian(0.0f);

		return _getInternal()->getSwingZ();
	}

	LimitLinear CD6Joint::getLimitLinear() const
	{
		return mLimitLinear;
	}

	void CD6Joint::setLimitLinear(const LimitLinear& limit)
	{
		if (mLimitLinear == limit)
			return;

		mLimitLinear = limit;

		if (mInternal != nullptr)
			_getInternal()->setLimitLinear(limit);
	}

	LimitAngularRange CD6Joint::getLimitTwist() const
	{
		return mLimitTwist;
	}

	void CD6Joint::setLimitTwist(const LimitAngularRange& limit)
	{
		if (mLimitTwist == limit)
			return;

		mLimitTwist = limit;

		if (mInternal != nullptr)
			_getInternal()->setLimitTwist(limit);
	}

	LimitConeRange CD6Joint::getLimitSwing() const
	{
		return mLimitSwing;
	}

	void CD6Joint::setLimitSwing(const LimitConeRange& limit)
	{
		if (mLimitSwing == limit)
			return;

		mLimitSwing = limit;

		if (mInternal != nullptr)
			_getInternal()->setLimitSwing(limit);
	}

	D6Joint::Drive CD6Joint::getDrive(D6Joint::DriveType type) const
	{
		return mDrive[(int)type];
	}

	void CD6Joint::setDrive(D6Joint::DriveType type, const D6Joint::Drive& drive)
	{
		if (mDrive[(int)type] == drive)
			return;

		mDrive[(int)type] = drive;

		if (mInternal != nullptr)
			_getInternal()->setDrive(type, drive);
	}

	Vector3 CD6Joint::getDrivePosition() const
	{
		return mDrivePosition;
	}

	Quaternion CD6Joint::getDriveRotation() const
	{
		return mDriveRotation;
	}

	void CD6Joint::setDriveTransform(const Vector3& position, const Quaternion& rotation)
	{
		if (mDrivePosition == position && mDriveRotation == rotation)
			return;

		mDrivePosition = position;
		mDriveRotation = rotation;

		if (mInternal != nullptr)
			_getInternal()->setDriveTransform(position, rotation);
	}

	Vector3 CD6Joint::getDriveLinearVelocity() const
	{
		return mDriveLinearVelocity;
	}

	Vector3 CD6Joint::getDriveAngularVelocity() const
	{
		return mDriveAngularVelocity;
	}

	void CD6Joint::setDriveVelocity(const Vector3& linear, const Vector3& angular)
	{
		if (mDriveLinearVelocity == linear && mDriveAngularVelocity == angular)
			return;

		mDriveLinearVelocity = linear;
		mDriveAngularVelocity = angular;

		if (mInternal != nullptr)
			_getInternal()->setDriveVelocity(linear, angular);
	}

	SPtr<Joint> CD6Joint::createInternal()
	{
		SPtr<Joint> joint = D6Joint::create();

		joint->_setOwner(PhysicsOwnerType::Component, this);
		return joint;
	}

	RTTITypeBase* CD6Joint::getRTTIStatic()
	{
		return CD6JointRTTI::instance();
	}

	RTTITypeBase* CD6Joint::getRTTI() const
	{
		return CD6Joint::getRTTIStatic();
	}
}
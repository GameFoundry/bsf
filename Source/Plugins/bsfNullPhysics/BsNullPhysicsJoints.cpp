//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsNullPhysicsJoints.h"
#include "BsNullPhysicsRigidbody.h"

namespace bs
{
	FNullPhysicsJoint::FNullPhysicsJoint(const JOINT_DESC& desc)
		:FJoint(desc), mDesc(desc)
	{ }

	void FNullPhysicsJoint::setTransform(JointBody body, const Vector3& position, const Quaternion& rotation)
	{
		mDesc.bodies[(int)body].position = position;
		mDesc.bodies[(int)body].rotation = rotation;
	}

	NullPhysicsD6Joint::NullPhysicsD6Joint(const D6_JOINT_DESC& desc)
		:D6Joint(desc)
	{
		mInternal = bs_new<FNullPhysicsJoint>(desc);
	}

	NullPhysicsD6Joint::~NullPhysicsD6Joint()
	{
		bs_delete(mInternal);
	}

	void NullPhysicsD6Joint::setDriveTransform(const Vector3& position, const Quaternion& rotation)
	{
		mDesc.drivePosition = position;
		mDesc.driveRotation = rotation;
	}

	void NullPhysicsD6Joint::setDriveVelocity(const Vector3& linear, const Vector3& angular)
	{
		mDesc.driveLinearVelocity = linear;
		mDesc.driveAngularVelocity = angular;
	}

	NullPhysicsDistanceJoint::NullPhysicsDistanceJoint(const DISTANCE_JOINT_DESC& desc)
		:DistanceJoint(desc)
	{
		mInternal = bs_new<FNullPhysicsJoint>(desc);
	}

	NullPhysicsDistanceJoint::~NullPhysicsDistanceJoint()
	{
		bs_delete(mInternal);
	}

	void NullPhysicsDistanceJoint::setFlag(DistanceJointFlag flag, bool enabled)
	{
		int flags = (int)mDesc.flag;
		int newFlag = (int)flag;

		if(enabled)
			flags |= newFlag;
		else
			flags &= ~newFlag;

		mDesc.flag = (DistanceJointFlag)flags;
	}

	bool NullPhysicsDistanceJoint::hasFlag(DistanceJointFlag flag) const
	{
		return ((int)mDesc.flag & (int)flag) != 0;
	}

	NullPhysicsFixedJoint::NullPhysicsFixedJoint(const FIXED_JOINT_DESC& desc)
		:FixedJoint(desc)
	{
		mInternal = bs_new<FNullPhysicsJoint>(desc);
	}

	NullPhysicsFixedJoint::~NullPhysicsFixedJoint()
	{
		bs_delete(mInternal);
	}

	NullPhysicsHingeJoint::NullPhysicsHingeJoint(const HINGE_JOINT_DESC& desc)
		:HingeJoint(desc), mDesc(desc)
	{
		mInternal = bs_new<FNullPhysicsJoint>(desc);
	}

	NullPhysicsHingeJoint::~NullPhysicsHingeJoint()
	{
		bs_delete(mInternal);
	}

	void NullPhysicsHingeJoint::setFlag(HingeJointFlag flag, bool enabled)
	{
		int flags = (int)mDesc.flag;
		int newFlag = (int)flag;

		if(enabled)
			flags |= newFlag;
		else
			flags &= ~newFlag;

		mDesc.flag = (HingeJointFlag)flags;
	}

	bool NullPhysicsHingeJoint::hasFlag(HingeJointFlag flag) const
	{
		return ((int)mDesc.flag & (int)flag) != 0;
	}

	NullPhysicsSliderJoint::NullPhysicsSliderJoint(const SLIDER_JOINT_DESC& desc)
		:SliderJoint(desc), mDesc(desc)
	{
		mInternal = bs_new<FNullPhysicsJoint>(desc);
	}

	NullPhysicsSliderJoint::~NullPhysicsSliderJoint()
	{
		bs_delete(mInternal);
	}

	void NullPhysicsSliderJoint::setFlag(SliderJointFlag flag, bool enabled)
	{
		int flags = (int)mDesc.flag;
		int newFlag = (int)flag;

		if(enabled)
			flags |= newFlag;
		else
			flags &= ~newFlag;

		mDesc.flag = (SliderJointFlag)flags;
	}

	bool NullPhysicsSliderJoint::hasFlag(SliderJointFlag flag) const
	{
		return ((int)mDesc.flag & (int)flag) != 0;
	}

	NullPhysicsSphericalJoint::NullPhysicsSphericalJoint(const SPHERICAL_JOINT_DESC& desc)
		:SphericalJoint(desc), mDesc(desc)
	{
		mInternal = bs_new<FNullPhysicsJoint>(desc);
	}

	NullPhysicsSphericalJoint::~NullPhysicsSphericalJoint()
	{
		bs_delete(mInternal);
	}

	void NullPhysicsSphericalJoint::setFlag(SphericalJointFlag flag, bool enabled)
	{
		int flags = (int)mDesc.flag;
		int newFlag = (int)flag;

		if(enabled)
			flags |= newFlag;
		else
			flags &= ~newFlag;

		mDesc.flag = (SphericalJointFlag)flags;
	}

	bool NullPhysicsSphericalJoint::hasFlag(SphericalJointFlag flag) const
	{
		return ((int)mDesc.flag & (int)flag) != 0;
	}
}

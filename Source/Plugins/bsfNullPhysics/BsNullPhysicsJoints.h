//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsNullPhysicsPrerequisites.h"
#include "Physics/BsJoint.h"
#include "Physics/BsFJoint.h"
#include "Physics/BsD6Joint.h"
#include "Physics/BsDistanceJoint.h"
#include "Physics/BsFixedJoint.h"
#include "Physics/BsHingeJoint.h"
#include "Physics/BsSliderJoint.h"
#include "Physics/BsSphericalJoint.h"

namespace bs
{
	/** @addtogroup NullPhysics
	 *  @{
	 */

	/** Null implementation of an FJoint. */
	class FNullPhysicsJoint : public FJoint
	{
	public:
		FNullPhysicsJoint(const JOINT_DESC& desc);
		~FNullPhysicsJoint() = default;

		/** @copydoc FJoint::getBody */
		Rigidbody* getBody(JointBody body) const override { return mDesc.bodies[(int)body].body; }


		/** @copydoc FJoint::setBody */
		void setBody(JointBody body, Rigidbody* value) override { mDesc.bodies[(int)body].body = value; }

		/** @copydoc FJoint::getPosition */
		Vector3 getPosition(JointBody body) const override { return mDesc.bodies[(int)body].position; }

		/** @copydoc FJoint::getRotation */
		Quaternion getRotation(JointBody body) const override { return mDesc.bodies[(int)body].rotation; }

		/** @copydoc FJoint::setTransform */
		void setTransform(JointBody body, const Vector3& position, const Quaternion& rotation) override;

		/** @copydoc FJoint::getBreakForce */
		float getBreakForce() const override { return mDesc.breakForce; }

		/** @copydoc FJoint::setBreakForce */
		void setBreakForce(float force) override { mDesc.breakForce = force; }

		/** @copydoc FJoint::getBreakTorque */
		float getBreakTorque() const override { return mDesc.breakTorque; }

		/** @copydoc FJoint::setBreakTorque */
		void setBreakTorque(float torque) override { mDesc.breakTorque = torque; }

		/** @copydoc FJoint::getEnableCollision */
		bool getEnableCollision() const override { return mDesc.enableCollision; }

		/** @copydoc FJoint::setEnableCollision */
		void setEnableCollision(bool value) override { mDesc.enableCollision = value; }

	private:
		JOINT_DESC mDesc;
	};

	/** NullPhysics implementation of a D6 joint. */
	class NullPhysicsD6Joint : public D6Joint
	{
	public:
		NullPhysicsD6Joint(const D6_JOINT_DESC& desc);
		~NullPhysicsD6Joint();

		/** @copydoc D6Joint::getMotion */
		D6JointMotion getMotion(D6JointAxis axis) const override { return mDesc.motion[(int)axis]; }

		/** @copydoc D6Joint::setMotion */
		void setMotion(D6JointAxis axis, D6JointMotion motion) override { mDesc.motion[(int)axis] = motion; }

		/** @copydoc D6Joint::getTwist */
		Radian getTwist() const override { return Radian(0.0f); }

		/** @copydoc D6Joint::getSwingY */
		Radian getSwingY() const override { return Radian(0.0f); }

		/** @copydoc D6Joint::getSwingZ */
		Radian getSwingZ() const override { return Radian(0.0f); }

		/** @copydoc D6Joint::getLimitLinear */
		LimitLinear getLimitLinear() const override { return mDesc.limitLinear; }

		/** @copydoc D6Joint::setLimitLinear */
		void setLimitLinear(const LimitLinear& limit) override { mDesc.limitLinear = limit; }

		/** @copydoc D6Joint::getLimitTwist */
		LimitAngularRange getLimitTwist() const override { return mDesc.limitTwist; }

		/** @copydoc D6Joint::setLimitTwist */
		void setLimitTwist(const LimitAngularRange& limit) override { mDesc.limitTwist = limit; }

		/** @copydoc D6Joint::getLimitSwing */
		LimitConeRange getLimitSwing() const override { return mDesc.limitSwing; }

		/** @copydoc D6Joint::setLimitSwing */
		void setLimitSwing(const LimitConeRange& limit) override { mDesc.limitSwing = limit; }

		/** @copydoc D6Joint::getDrive */
		D6JointDrive getDrive(D6JointDriveType type) const override { return mDesc.drive[(int)type]; }

		/** @copydoc D6Joint::setDrive */
		void setDrive(D6JointDriveType type, const D6JointDrive& drive) override { mDesc.drive[(int)type] = drive; }

		/** @copydoc D6Joint::getDrivePosition */
		Vector3 getDrivePosition() const override { return mDesc.drivePosition; }

		/** @copydoc D6Joint::getDriveRotation */
		Quaternion getDriveRotation() const override { return mDesc.driveRotation; }

		/** @copydoc D6Joint::setDriveTransform */
		void setDriveTransform(const Vector3& position, const Quaternion& rotation) override;

		/** @copydoc D6Joint::getDriveLinearVelocity */
		Vector3 getDriveLinearVelocity() const override { return mDesc.driveLinearVelocity; }

		/** @copydoc D6Joint::getDriveAngularVelocity */
		Vector3 getDriveAngularVelocity() const override { return mDesc.driveAngularVelocity; }

		/** @copydoc D6Joint::setDriveVelocity */
		void setDriveVelocity(const Vector3& linear, const Vector3& angular) override;

	private:
		D6_JOINT_DESC mDesc;
	};

	/** Null implementation of a DistanceJoint */
	class NullPhysicsDistanceJoint : public DistanceJoint
	{
	public:
		NullPhysicsDistanceJoint(const DISTANCE_JOINT_DESC& desc);
		~NullPhysicsDistanceJoint();

		/** @copydoc DistanceJoint::getDistance */
		float getDistance() const override { return 0.0f; }

		/** @copydoc DistanceJoint::getMinDistance */
		float getMinDistance() const override { return mDesc.minDistance; }

		/** @copydoc DistanceJoint::setMinDistance */
		void setMinDistance(float value) override { mDesc.minDistance = value; }

		/** @copydoc DistanceJoint::getMaxDistance */
		float getMaxDistance() const override { return mDesc.maxDistance; }

		/** @copydoc DistanceJoint::setMaxDistance */
		void setMaxDistance(float value) override { mDesc.maxDistance = value; }

		/** @copydoc DistanceJoint::getTolerance */
		float getTolerance() const override { return mDesc.tolerance; }

		/** @copydoc DistanceJoint::setTolerance */
		void setTolerance(float value) override { mDesc.tolerance = value; }

		/** @copydoc DistanceJoint::getSpring */
		Spring getSpring() const override { return mDesc.spring; }

		/** @copydoc DistanceJoint::setSpring */
		void setSpring(const Spring& value) override { mDesc.spring = value; }

		/** @copydoc DistanceJoint::setFlag */
		void setFlag(DistanceJointFlag flag, bool enabled) override;

		/** @copydoc DistanceJoint::hasFlag */
		bool hasFlag(DistanceJointFlag flag) const override;

	private:
		DISTANCE_JOINT_DESC mDesc;
	};

	/** Null implementation of a FixedJoint. */
	class NullPhysicsFixedJoint : public FixedJoint
	{
	public:
		NullPhysicsFixedJoint(const FIXED_JOINT_DESC& desc);
		~NullPhysicsFixedJoint();
	};

	/** Null implementation of a HingeJoint. */
	class NullPhysicsHingeJoint : public HingeJoint
	{
	public:
		NullPhysicsHingeJoint(const HINGE_JOINT_DESC& desc);
		~NullPhysicsHingeJoint();

		/** @copydoc HingeJoint::getAngle */
		Radian getAngle() const override { return Radian(0.0f); }

		/** @copydoc HingeJoint::getSpeed */
		float getSpeed() const override { return 0.0f; }

		/** @copydoc HingeJoint::getLimit */
		LimitAngularRange getLimit() const override { return mDesc.limit; }

		/** @copydoc HingeJoint::setLimit */
		void setLimit(const LimitAngularRange& limit) override { mDesc.limit = limit; }

		/** @copydoc HingeJoint::getDrive */
		HingeJointDrive getDrive() const override { return mDesc.drive; }

		/** @copydoc HingeJoint::setDrive */
		void setDrive(const HingeJointDrive& drive) override { mDesc.drive = drive; }

		/** @copydoc HingeJoint::setFlag */
		void setFlag(HingeJointFlag flag, bool enabled) override;

		/** @copydoc HingeJoint::hasFlag */
		bool hasFlag(HingeJointFlag flag) const override;

	private:
		HINGE_JOINT_DESC mDesc;
	};

	/** Null implementation of a SliderJoint. */
	class NullPhysicsSliderJoint : public SliderJoint
	{
	public:
		NullPhysicsSliderJoint(const SLIDER_JOINT_DESC& desc);
		~NullPhysicsSliderJoint();

		/** @copydoc SliderJoint::getPosition */
		float getPosition() const override { return 0.0f; }

		/** @copydoc SliderJoint::getSpeed */
		float getSpeed() const override { return 0.0f; }

		/** @copydoc SliderJoint::getLimit */
		LimitLinearRange getLimit() const override { return mDesc.limit; }

		/** @copydoc SliderJoint::setLimit */
		void setLimit(const LimitLinearRange& limit) override { mDesc.limit = limit; }

		/** @copydoc SliderJoint::setFlag */
		void setFlag(SliderJointFlag flag, bool enabled) override;

		/** @copydoc SliderJoint::hasFlag */
		bool hasFlag(SliderJointFlag flag) const override;

	private:
		SLIDER_JOINT_DESC mDesc;
	};

	/** Null implementation of a SphericalJoint. */
	class NullPhysicsSphericalJoint : public SphericalJoint
	{
	public:
		NullPhysicsSphericalJoint(const SPHERICAL_JOINT_DESC& desc);
		~NullPhysicsSphericalJoint();

		/** @copydoc SphericalJoint::getLimit */
		LimitConeRange getLimit() const override { return mDesc.limit; }

		/** @copydoc SphericalJoint::setLimit */
		void setLimit(const LimitConeRange& limit) override { mDesc.limit = limit; }

		/** @copydoc SphericalJoint::setFlag */
		void setFlag(SphericalJointFlag flag, bool enabled) override;

		/** @copydoc SphericalJoint::hasFlag */
		bool hasFlag(SphericalJointFlag flag) const override;

	private:
		SPHERICAL_JOINT_DESC mDesc;
	};

	/** @} */
}

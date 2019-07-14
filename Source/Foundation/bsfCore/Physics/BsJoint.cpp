//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Physics/BsJoint.h"

namespace bs
{
	Rigidbody* Joint::getBody(JointBody body) const
	{
		return mInternal->getBody(body);
	}

	void Joint::setBody(JointBody body, Rigidbody* value)
	{
		mInternal->setBody(body, value);
	}

	Vector3 Joint::getPosition(JointBody body) const
	{
		return mInternal->getPosition(body);
	}

	Quaternion Joint::getRotation(JointBody body) const
	{
		return mInternal->getRotation(body);
	}

	void Joint::setTransform(JointBody body, const Vector3& position, const Quaternion& rotation)
	{
		mInternal->setTransform(body, position, rotation);
	}

	float Joint::getBreakForce() const
	{
		return mInternal->getBreakForce();
	}

	void Joint::setBreakForce(float force)
	{
		mInternal->setBreakForce(force);
	}

	float Joint::getBreakTorque() const
	{
		return mInternal->getBreakTorque();
	}

	void Joint::setBreakTorque(float torque)
	{
		mInternal->setBreakTorque(torque);
	}

	bool Joint::getEnableCollision() const
	{
		return mInternal->getEnableCollision();
	}

	void Joint::setEnableCollision(bool value)
	{
		mInternal->setEnableCollision(value);
	}		
}

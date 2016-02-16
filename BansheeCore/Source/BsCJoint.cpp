//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsCJoint.h"
#include "BsCRigidbody.h"
#include "BsSceneObject.h"
#include "BsCJointRTTI.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	CJoint::CJoint(const HSceneObject& parent)
		: Component(parent)
	{
		setName("Joint");

		mNotifyFlags = (TransformChangedFlags)(TCF_Parent | TCF_Transform);
	}

	HRigidbody CJoint::getBody(JointBody body) const
	{
		return mBodies[(int)body];
	}

	void CJoint::setBody(JointBody body, const HRigidbody& value)
	{
		if (mBodies[(int)body] == value)
			return;

		mBodies[(int)body] = value;

		if(mInternal != nullptr)
		{
			Rigidbody* rigidbody = nullptr;
			if (value != nullptr)
				rigidbody = value->_getInternal().get();

			mInternal->setBody(body, rigidbody);
			updateTransform(body);
		}
	}

	Vector3 CJoint::getPosition(JointBody body) const
	{
		return mPositions[(int)body];
	}

	Quaternion CJoint::getRotation(JointBody body) const
	{
		return mRotations[(int)body];
	}

	void CJoint::setTransform(JointBody body, const Vector3& position, const Quaternion& rotation)
	{
		if (mPositions[(int)body] == position && mRotations[(int)body] == rotation)
			return;

		mPositions[(int)body] = position;
		mRotations[(int)body] = rotation;

		if(mInternal != nullptr)
			updateTransform(body);
	}

	float CJoint::getBreakForce() const
	{
		return mBreakForce;
	}

	void CJoint::setBreakForce(float force)
	{
		if (mBreakForce == force)
			return;

		mBreakForce = force;

		if (mInternal != nullptr)
			mInternal->setBreakForce(force);
	}

	float CJoint::getBreakTorque() const
	{
		return mBreakTorque;
	}

	void CJoint::setBreakToque(float torque)
	{
		if (mBreakTorque == torque)
			return;

		mBreakTorque = torque;

		if (mInternal != nullptr)
			mInternal->setBreakTorque(torque);
	}

	bool CJoint::getEnableCollision() const
	{
		return mEnableCollision;
	}

	void CJoint::setEnableCollision(bool value)
	{
		if (mEnableCollision == value)
			return;

		mEnableCollision = value;

		if (mInternal != nullptr)
			mInternal->setEnableCollision(value);
	}

	void CJoint::onInitialized()
	{

	}

	void CJoint::onDestroyed()
	{
		// This should release the last reference and destroy the internal joint
		mInternal = nullptr;
	}

	void CJoint::onDisabled()
	{
		// This should release the last reference and destroy the internal joint
		mInternal = nullptr;
	}

	void CJoint::onEnabled()
	{
		restoreInternal();
	}

	void CJoint::onTransformChanged(TransformChangedFlags flags)
	{
		if (!SO()->getActive())
			return;

		// TODO - This might get called during physics update, perhaps avoid that?
		updateTransform(JointBody::A);
		updateTransform(JointBody::B);
	}

	void CJoint::restoreInternal()
	{
		if (mInternal == nullptr)
		{
			mInternal = createInternal();

			mInternal->onJointBreak.connect(std::bind(&CJoint::triggerOnJointBroken, this));
		}

		// Note: Merge into one call to avoid many virtual function calls
		Rigidbody* bodies[2];

		if (mBodies[0] != nullptr)
			bodies[0] = mBodies[0]->_getInternal().get();
		else
			bodies[0] = nullptr;

		if (mBodies[1] != nullptr)
			bodies[1] = mBodies[1]->_getInternal().get();
		else
			bodies[1] = nullptr;

		mInternal->setBody(JointBody::A, bodies[0]);
		mInternal->setBody(JointBody::B, bodies[1]);
		mInternal->setBreakForce(mBreakForce);
		mInternal->setBreakTorque(mBreakTorque);
		mInternal->setEnableCollision(mEnableCollision);

		updateTransform(JointBody::A);
		updateTransform(JointBody::B);
	}

	void CJoint::updateTransform(JointBody body)
	{
		Vector3 localPos;
		Quaternion localRot;

		localPos = mPositions[(int)body];
		localRot = mRotations[(int)body];

		// Transform to world space of the related body
		HRigidbody rigidbody = mBodies[(int)body];
		if(rigidbody != nullptr)
		{
			localRot = rigidbody->SO()->getWorldRotation() * localRot;
			localPos = localRot.rotate(localPos) + rigidbody->SO()->getWorldPosition();
		}

		// Transform to space local to the joint
		Quaternion invRotation = SO()->getWorldRotation().inverse();

		localPos = invRotation.rotate(localPos - SO()->getWorldPosition());
		localRot = invRotation * localRot;

		mInternal->setTransform(body, localPos, localRot);
	}

	void CJoint::triggerOnJointBroken()
	{
		onJointBreak();
	}

	RTTITypeBase* CJoint::getRTTIStatic()
	{
		return CJointRTTI::instance();
	}

	RTTITypeBase* CJoint::getRTTI() const
	{
		return CJoint::getRTTIStatic();
	}
}
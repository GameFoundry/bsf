//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsCJoint.h"
#include "BsCRigidbody.h"
#include "BsSceneObject.h"
#include "BsPhysics.h"
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

		if (mBodies[(int)body] != nullptr)
			mBodies[(int)body]->_setJoint(HJoint());

		mBodies[(int)body] = value;

		if (value != nullptr)
			mBodies[(int)body]->_setJoint(mThisHandle);

		if(mInternal != nullptr)
		{
			Rigidbody* rigidbody = nullptr;
			if (value != nullptr)
				rigidbody = value->_getInternal();

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
		if (mBodies[0] != nullptr)
			mBodies[0]->_setJoint(HJoint());

		if (mBodies[1] != nullptr)
			mBodies[1]->_setJoint(HJoint());

		destroyInternal();
	}

	void CJoint::onDisabled()
	{
		destroyInternal();
	}

	void CJoint::onEnabled()
	{
		restoreInternal();
	}

	void CJoint::onTransformChanged(TransformChangedFlags flags)
	{
		if (!SO()->getActive())
			return;

		// We're ignoring this during physics update because it would cause problems if the joint itself was moved by physics
		// Note: This isn't particularily correct because if the joint is being moved by physics but the rigidbodies
		// themselves are not parented to the joint, the transform will need updating. However I'm leaving it up to the
		// user to ensure rigidbodies are always parented to the joint in such a case (It's an unlikely situation that
		// I can't think of an use for - joint transform will almost always be set as an initialization step and not a 
		// physics response).
		if (gPhysics()._isUpdateInProgress())
			return;

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
			bodies[0] = mBodies[0]->_getInternal();
		else
			bodies[0] = nullptr;

		if (mBodies[1] != nullptr)
			bodies[1] = mBodies[1]->_getInternal();
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

	void CJoint::destroyInternal()
	{
		// This should release the last reference and destroy the internal joint
		mInternal->_setOwner(PhysicsOwnerType::None, nullptr);
		mInternal = nullptr;
	}

	void CJoint::notifyRigidbodyMoved(const HRigidbody& body)
	{
		// If physics update is in progress do nothing, as its the joint itself that's probably moving the body
		if (gPhysics()._isUpdateInProgress())
			return;

		if (mBodies[0] == body)
			updateTransform(JointBody::A);
		else if (mBodies[1] == body)
			updateTransform(JointBody::B);
		else
			assert(false); // Not allowed to happen
	}

	void CJoint::updateTransform(JointBody body)
	{
		Vector3 parentPos = SO()->getWorldPosition();
		Quaternion parentRot = SO()->getWorldRotation();
		
		// Add local position/rotation offset to the joint's transform
		Vector3 worldPos = parentPos + parentRot.rotate(mPositions[(int)body]);
		Quaternion worldRot = parentRot * mRotations[(int)body];

		// Transform body's world position/rotation into space local to the joint + offset space
		Vector3 bodyPos;
		Quaternion bodyRot;

		HRigidbody rigidbody = mBodies[(int)body];
		if(rigidbody != nullptr)
		{
			bodyPos = rigidbody->SO()->getWorldPosition(); 
			bodyRot = rigidbody->SO()->getWorldRotation();
		}

		Quaternion invRotation = worldRot.inverse();

		bodyPos = invRotation.rotate(bodyPos - worldPos);
		bodyRot = invRotation * bodyRot;

		mInternal->setTransform(body, bodyPos, bodyRot);
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
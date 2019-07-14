//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsNullPhysicsPrerequisites.h"
#include "Physics/BsRigidbody.h"
#include "Math/BsVector3.h"
#include "Math/BsQuaternion.h"

namespace bs
{
	/** @addtogroup NullPhysics
	 *  @{
	 */

	/** Null implementation of a Rigidbody. */
	class NullPhysicsRigidbody : public Rigidbody
	{
	public:
		NullPhysicsRigidbody(const HSceneObject& linkedSO);
		~NullPhysicsRigidbody() = default;

		/** @copydoc Rigidbody::move */
		void move(const Vector3& position) override;

		/** @copydoc Rigidbody::rotate */
		void rotate(const Quaternion& rotation) override;

		/** @copydoc Rigidbody::getPosition */
		Vector3 getPosition() const override;

		/** @copydoc Rigidbody::getRotation */
		Quaternion getRotation() const override;

		/** @copydoc Rigidbody::setTransform */
		void setTransform(const Vector3& pos, const Quaternion& rot) override;

		/** @copydoc Rigidbody::setMass */
		void setMass(float mass) override { mMass = mass; }

		/** @copydoc Rigidbody::getMass */
		float getMass() const override { return mMass; }

		/** @copydoc Rigidbody::setIsKinematic */
		void setIsKinematic(bool kinematic) override { mIsKinematic = kinematic; }

		/** @copydoc Rigidbody::getIsKinematic */
		bool getIsKinematic() const override { return mIsKinematic; }

		/** @copydoc Rigidbody::isSleeping */
		bool isSleeping() const override { return false; }

		/** @copydoc Rigidbody::sleep */
		void sleep() override { }

		/** @copydoc Rigidbody::wakeUp */
		void wakeUp() override { }

		/** @copydoc Rigidbody::setSleepThreshold */
		void setSleepThreshold(float threshold) override { mSleepThreshold = threshold; }

		/** @copydoc Rigidbody::getSleepThreshold */
		float getSleepThreshold() const override { return mSleepThreshold; }

		/** @copydoc Rigidbody::setUseGravity */
		void setUseGravity(bool gravity) override { mUseGravity = gravity; }

		/** @copydoc Rigidbody::getUseGravity */
		bool getUseGravity() const override { return mUseGravity; }

		/** @copydoc Rigidbody::setVelocity */
		void setVelocity(const Vector3& velocity) override { mVelocity = velocity; }

		/** @copydoc Rigidbody::getVelocity */
		Vector3 getVelocity() const override { return mVelocity; }

		/** @copydoc Rigidbody::setAngularVelocity */
		void setAngularVelocity(const Vector3& velocity) override { mAngularVelocity = velocity; }

		/** @copydoc Rigidbody::getAngularVelocity */
		Vector3 getAngularVelocity() const override { return mAngularVelocity; }

		/** @copydoc Rigidbody::setDrag */
		void setDrag(float drag) override { mDrag = drag; }

		/** @copydoc Rigidbody::getDrag */
		float getDrag() const override { return mDrag; }

		/** @copydoc Rigidbody::setAngularDrag */
		void setAngularDrag(float drag) override { mAngularDrag = drag; }

		/** @copydoc Rigidbody::getAngularDrag */
		float getAngularDrag() const override { return mAngularDrag; }

		/** @copydoc Rigidbody::setInertiaTensor */
		void setInertiaTensor(const Vector3& tensor) override { mInertiaTensor = tensor; }

		/** @copydoc Rigidbody::getInertiaTensor */
		Vector3 getInertiaTensor() const override { return mInertiaTensor; }

		/** @copydoc Rigidbody::setMaxAngularVelocity */
		void setMaxAngularVelocity(float maxVelocity) override { mMaxAngularVelocity = maxVelocity; }

		/** @copydoc Rigidbody::getMaxAngularVelocity */
		float getMaxAngularVelocity() const override { return mMaxAngularVelocity; }

		/** @copydoc Rigidbody::setCenterOfMass */
		void setCenterOfMass(const Vector3& position, const Quaternion& rotation) override;

		/** @copydoc Rigidbody::getCenterOfMassPosition */
		Vector3 getCenterOfMassPosition() const override { return mCenterOfMassPosition; }

		/** @copydoc Rigidbody::getCenterOfMassRotation */
		Quaternion getCenterOfMassRotation() const override { return mCenterOfMassRotation; }

		/** @copydoc Rigidbody::setPositionSolverCount */
		void setPositionSolverCount(UINT32 count) override { mPositionSolverCount = count; }

		/** @copydoc Rigidbody::getPositionSolverCount */
		UINT32 getPositionSolverCount() const override { return mPositionSolverCount; }

		/** @copydoc Rigidbody::setVelocitySolverCount */
		void setVelocitySolverCount(UINT32 count) override { mVelocitySolverCount = count; }

		/** @copydoc Rigidbody::getVelocitySolverCount */
		UINT32 getVelocitySolverCount() const override { return mVelocitySolverCount; }

		/** @copydoc Rigidbody::addForce */
		void addForce(const Vector3& force, ForceMode mode = ForceMode::Force) override { }

		/** @copydoc Rigidbody::addTorque */
		void addTorque(const Vector3& torque, ForceMode mode = ForceMode::Force) override { }

		/** @copydoc Rigidbody::addForceAtPoint */
		void addForceAtPoint(const Vector3& force, const Vector3& position,
			PointForceMode mode = PointForceMode::Force) override { }

		/** @copydoc Rigidbody::getVelocityAtPoint */
		Vector3 getVelocityAtPoint(const Vector3& point) const override { return Vector3::ZERO; }

		/** @copydoc Rigidbody::addCollider */
		void addCollider(Collider* collider) override { }

		/** @copydoc Rigidbody::removeCollider */
		void removeCollider(Collider* collider) override { }

		/** @copydoc Rigidbody::removeColliders */
		void removeColliders() override { }
		
	private:
		Vector3 mPosition = Vector3::ZERO;
		Quaternion mRotation = Quaternion::IDENTITY;
		float mMass = 0.0f;
		bool mIsKinematic = false;
		float mSleepThreshold = 0.1f;
		bool mUseGravity = false;
		Vector3 mVelocity = Vector3::ZERO;
		Vector3 mAngularVelocity = Vector3::ZERO;
		float mDrag = 0.0f;
		float mAngularDrag = 0.0f;
		Vector3 mInertiaTensor = Vector3::ZERO;
		float mMaxAngularVelocity = std::numeric_limits<float>::max();
		Vector3 mCenterOfMassPosition = Vector3::ZERO;
		Quaternion mCenterOfMassRotation = Quaternion::IDENTITY;
		UINT32 mPositionSolverCount = 0;
		UINT32 mVelocitySolverCount = 0;
	};

	/** @} */
}

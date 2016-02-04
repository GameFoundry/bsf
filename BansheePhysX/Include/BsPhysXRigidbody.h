//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPhysXPrerequisites.h"
#include "BsRigidbody.h"
#include "BsVector3.h"
#include "BsQuaternion.h"
#include "PxPhysics.h"

namespace BansheeEngine
{
	class PhysXRigidbody : public Rigidbody
	{
	public:
		PhysXRigidbody(physx::PxPhysics* physx, physx::PxScene* scene, const Vector3& position, 
			const Quaternion& rotation, UINT32 priority);
		~PhysXRigidbody();

		void move(const Vector3& position) override;
		void rotate(const Quaternion& rotation) override;

		Vector3 getPosition() const override;
		Quaternion getRotation() const override;
		void setTransform(const Vector3& pos, const Quaternion& rot) override;

		void setMass(float mass) override;
		float getMass() const override;

		void setIsKinematic(bool kinematic) override;
		bool getIsKinematic() const override;

		bool isSleeping() const override;
		void sleep() override;
		void wakeUp() override;

		void setSleepThreshold(float threshold) override;
		float getSleepThreshold() const override;

		void setUseGravity(bool gravity) override;
		bool getUseGravity() const override;

		void setVelocity(const Vector3& velocity) override;
		Vector3 getVelocity() const override;

		void setAngularVelocity(const Vector3& velocity) override;
		Vector3 getAngularVelocity() const override;

		void setDrag(float drag) override;
		float getDrag() const override;

		void setAngularDrag(float drag) override;
		float getAngularDrag() const override;

		void setInertiaTensor(const Vector3& tensor) override;
		Vector3 getInertiaTensor() const override;

		void setMaxAngularVelocity(float maxVelocity) override;
		float getMaxAngularVelocity() const override;

		void setCenterOfMass(const Vector3& position, const Quaternion& rotation) override;
		Vector3 getCenterOfMassPosition() const override;
		Quaternion getCenterOfMassRotatation() const override;

		void setPositionSolverCount(UINT32 count) override;
		UINT32 getPositionSolverCount() const override;

		void setVelocitySolverCount(UINT32 count) override;
		UINT32 getVelocitySolverCount() const override;

		void setIsActive(bool value) override;

		void addForce(const Vector3& force, ForceMode mode = ForceMode::Force) override;
		void addTorque(const Vector3& torque, ForceMode mode = ForceMode::Force) override;
		void addForceAtPoint(const Vector3& force, const Vector3& position,
			PointForceMode mode = PointForceMode::Force) override;

		Vector3 getVelocityAtPoint(const Vector3& point) const override;

		void _updateMassDistribution() override;
	private:
		physx::PxRigidDynamic* mInternal;
	};
}
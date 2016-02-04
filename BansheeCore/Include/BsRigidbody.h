//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsCollision.h"

namespace BansheeEngine
{
	enum class ForceMode
	{
		Force, 
		Impulse,
		Velocity,
		Acceleration
	};

	enum class PointForceMode
	{
		Force,
		Impulse,
	};

	class BS_CORE_EXPORT Rigidbody
	{
	public:
		enum class Flag
		{
			None = 0x00,
			/** Automatically calculate center of mass transform and inertia tensors from child shapes (colliders) */
			AutoTensors = 0x01,
			/** Calculate mass distribution from child shapes (colliders). Only relevant when auto-tensors is on. */
			AutoMass = 0x02,
		};

		Rigidbody(UINT32 priority);
		virtual ~Rigidbody();

		virtual void move(const Vector3& position) = 0;
		virtual void rotate(const Quaternion& rotation) = 0;

		virtual Vector3 getPosition() const = 0;
		virtual Quaternion getRotation() const = 0;
		virtual void setTransform(const Vector3& pos, const Quaternion& rot) = 0;

		virtual void setMass(float mass) = 0;
		virtual float getMass() const = 0;

		virtual void setIsKinematic(bool kinematic) = 0;
		virtual bool getIsKinematic() const = 0;

		virtual bool isSleeping() const = 0;
		virtual void sleep() = 0;
		virtual void wakeUp() = 0;

		virtual void setSleepThreshold(float threshold) = 0;
		virtual float getSleepThreshold() const = 0;

		virtual void setUseGravity(bool gravity) = 0;
		virtual bool getUseGravity() const = 0;

		virtual void setVelocity(const Vector3& velocity) = 0;
		virtual Vector3 getVelocity() const = 0;

		virtual void setAngularVelocity(const Vector3& velocity) = 0;
		virtual Vector3 getAngularVelocity() const = 0;

		virtual void setDrag(float drag) = 0;
		virtual float getDrag() const = 0;

		virtual void setAngularDrag(float drag) = 0;
		virtual float getAngularDrag() const = 0;

		virtual void setInertiaTensor(const Vector3& tensor) = 0;
		virtual Vector3 getInertiaTensor() const = 0;

		virtual void setMaxAngularVelocity(float maxVelocity) = 0;
		virtual float getMaxAngularVelocity() const = 0;

		virtual void setCenterOfMass(const Vector3& position, const Quaternion& rotation) = 0;
		virtual Vector3 getCenterOfMassPosition() const = 0;
		virtual Quaternion getCenterOfMassRotatation() const = 0;

		virtual void setPositionSolverCount(UINT32 count) = 0;
		virtual UINT32 getPositionSolverCount() const = 0;

		virtual void setVelocitySolverCount(UINT32 count) = 0;
		virtual UINT32 getVelocitySolverCount() const = 0;

		virtual void setIsActive(bool value);
		virtual bool getIsActive() const { return mIsActive; }

		virtual void setFlags(Flag flags);
		virtual Flag getFlags() const { return mFlags; }

		virtual void addForce(const Vector3& force, ForceMode mode = ForceMode::Force) = 0;
		virtual void addTorque(const Vector3& torque, ForceMode mode = ForceMode::Force) = 0;
		virtual void addForceAtPoint(const Vector3& force, const Vector3& position, 
			PointForceMode mode = PointForceMode::Force) = 0;

		virtual Vector3 getVelocityAtPoint(const Vector3& point) const = 0;

		static SPtr<Rigidbody> create(const Vector3& position, const Quaternion& rotation);

		Event<void(const CollisionData&)> onCollisionBegin;
		Event<void(const CollisionData&)> onCollisionStay;
		Event<void(const CollisionData&)> onCollisionEnd;

		virtual void _updateMassDistribution() { }

		void _setPriority(UINT32 priority);
		void _setPhysicsId(UINT32 id) { mPhysicsId = id; }
	protected:
		friend class FCollider;

		virtual void addCollider(FCollider* collider);
		virtual void removeCollider(FCollider* collider);

		bool mIsActive = true;
		Flag mFlags = Flag::None;
		UINT32 mPriority = 0;
		UINT32 mPhysicsId = 0;
		Vector<FCollider*> mColliders;
	};
}
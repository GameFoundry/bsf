//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPhysXPrerequisites.h"
#include "BsPhysics.h"
#include "BSCollision.h"
#include "PxPhysics.h"
#include "foundation/Px.h"
#include "characterkinematic\PxControllerManager.h"
#include "cooking/PxCooking.h"

namespace BansheeEngine
{
	class PhysX : public Physics
	{
		enum class ContactEventType
		{
			ContactBegin,
			ContactStay,
			ContactEnd
		};

		struct TriggerEvent
		{
			Collider* trigger;
			Collider* other;
			ContactEventType type;
		};

		struct ContactEvent
		{
			Collider* colliderA;
			Collider* colliderB;
			ContactEventType type;
			Vector<ContactPoint> points; // Note: Not too happy this is heap allocated, use static allocator?
		};

		struct JointBreakEvent
		{
			Joint* joint;
		};

	public:
		PhysX();
		~PhysX();

		void update() override;

		SPtr<PhysicsMaterial> createMaterial(float staticFriction, float dynamicFriction, float restitution) override;
		SPtr<PhysicsMesh> createMesh(const MeshDataPtr& meshData, PhysicsMeshType type) override;
		SPtr<Rigidbody> createRigidbody(const HSceneObject& linkedSO) override;

		SPtr<BoxCollider> createBoxCollider(const Vector3& extents, const Vector3& position,
			const Quaternion& rotation) override;
		SPtr<SphereCollider> createSphereCollider(float radius, const Vector3& position, const Quaternion& rotation) override;
		SPtr<PlaneCollider> createPlaneCollider(const Vector3& position, const Quaternion& rotation) override;
		SPtr<CapsuleCollider> createCapsuleCollider(float radius, float halfHeight, const Vector3& position, 
			const Quaternion& rotation) override;
		SPtr<MeshCollider> createMeshCollider(const Vector3& position, const Quaternion& rotation) override;

		SPtr<FixedJoint> createFixedJoint() override;
		SPtr<DistanceJoint> createDistanceJoint() override;
		SPtr<HingeJoint> createHingeJoint() override;
		SPtr<SphericalJoint> createSphericalJoint() override;
		SPtr<SliderJoint> createSliderJoint() override;
		SPtr<D6Joint> createD6Joint() override;

		/** @copydoc Physics::createCharacterController*/
		SPtr<CharacterController> createCharacterController(const CHAR_CONTROLLER_DESC& desc) override;

		void _reportContactEvent(const ContactEvent& event);
		void _reportTriggerEvent(const TriggerEvent& event);
		void _reportJointBreakEvent(const JointBreakEvent& event);

		physx::PxMaterial* getDefaultMaterial() const { return mDefaultMaterial; }
		physx::PxPhysics* getPhysX() const { return mPhysics; }
		physx::PxScene* getScene() const { return mScene; }
		physx::PxCooking* getCooking() const { return mCooking; }
		physx::PxTolerancesScale getScale() const { return mScale; }

	private:
		friend class PhysXEventCallback;

		void triggerEvents();

		float mSimulationStep = 1.0f/60.0f;
		float mLastSimulationTime = 0.0f;

		Vector<TriggerEvent> mTriggerEvents;
		Vector<ContactEvent> mContactEvents;
		Vector<JointBreakEvent> mJointBreakEvents;

		physx::PxFoundation* mFoundation = nullptr;
		physx::PxPhysics* mPhysics = nullptr;
		physx::PxCooking* mCooking = nullptr;
		physx::PxScene* mScene = nullptr;
		physx::PxControllerManager* mCharManager = nullptr;

		physx::PxMaterial* mDefaultMaterial = nullptr;
		physx::PxTolerancesScale mScale;
	};

	/** Provides easier access to PhysX. */
	PhysX& gPhysX();
}
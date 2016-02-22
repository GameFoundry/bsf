//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPhysXPrerequisites.h"
#include "BsPhysics.h"
#include "BsPhysicsCommon.h"
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
		PhysX(const PHYSICS_INIT_DESC& input);
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

		/** @copydoc Physics::rayCast(const Vector3&, const Vector3&, UINT64, float) */
		bool rayCast(const Vector3& origin, const Vector3& unitDir, PhysicsQueryHit& hit,
			UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) override;

		/** @copydoc Physics::boxCast */
		bool boxCast(const AABox& box, const Quaternion& rotation, const Vector3& unitDir, PhysicsQueryHit& hit,
			UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) override;

		/** @copydoc Physics::sphereCast */
		bool sphereCast(const Sphere& sphere, const Vector3& unitDir, PhysicsQueryHit& hit,
			UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) override;

		/** @copydoc Physics::capsuleCast */
		bool capsuleCast(const Capsule& capsule, const Quaternion& rotation, const Vector3& unitDir,
			PhysicsQueryHit& hit, UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) override;

		/** @copydoc Physics::convexCast */
		bool convexCast(const HPhysicsMesh& mesh, const Vector3& position, const Quaternion& rotation,
			const Vector3& unitDir, PhysicsQueryHit& hit, UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) override;

		/** @copydoc Physics::rayCastAll(const Vector3&, const Vector3&, UINT64, float) */
		Vector<PhysicsQueryHit> rayCastAll(const Vector3& origin, const Vector3& unitDir,
			UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) override;

		/** @copydoc Physics::boxCastAll */
		Vector<PhysicsQueryHit> boxCastAll(const AABox& box, const Quaternion& rotation,
			const Vector3& unitDir, UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) override;

		/** @copydoc Physics::sphereCastAll */
		Vector<PhysicsQueryHit> sphereCastAll(const Sphere& sphere, const Vector3& unitDir,
			UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) override;

		/** @copydoc Physics::capsuleCastAll */
		Vector<PhysicsQueryHit> capsuleCastAll(const Capsule& capsule, const Quaternion& rotation,
			const Vector3& unitDir, UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) override;

		/** @copydoc Physics::convexCastAll */
		Vector<PhysicsQueryHit> convexCastAll(const HPhysicsMesh& mesh, const Vector3& position,
			const Quaternion& rotation, const Vector3& unitDir, UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) override;

		/** @copydoc Physics::rayCastAny(const Vector3&, const Vector3&, UINT64, float) */
		bool rayCastAny(const Vector3& origin, const Vector3& unitDir,
			UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) override;

		/** @copydoc Physics::boxCastAny */
		bool boxCastAny(const AABox& box, const Quaternion& rotation, const Vector3& unitDir,
			UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) override;

		/** @copydoc Physics::sphereCastAny */
		bool sphereCastAny(const Sphere& sphere, const Vector3& unitDir,
			UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) override;

		/** @copydoc Physics::capsuleCastAny */
		bool capsuleCastAny(const Capsule& capsule, const Quaternion& rotation, const Vector3& unitDir,
			UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) override;

		/** @copydoc Physics::convexCastAny */
		bool convexCastAny(const HPhysicsMesh& mesh, const Vector3& position, const Quaternion& rotation,
			const Vector3& unitDir, UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) override;

		/** @copydoc Physics::boxOverlapAny */
		bool boxOverlapAny(const AABox& box, const Quaternion& rotation, UINT64 layer = BS_ALL_LAYERS) override;

		/** @copydoc Physics::sphereOverlapAny */
		bool sphereOverlapAny(const Sphere& sphere, UINT64 layer = BS_ALL_LAYERS) override;

		/** @copydoc Physics::capsuleOverlapAny */
		bool capsuleOverlapAny(const Capsule& capsule, const Quaternion& rotation,
			UINT64 layer = BS_ALL_LAYERS) override;

		/** @copydoc Physics::convexOverlapAny */
		bool convexOverlapAny(const HPhysicsMesh& mesh, const Vector3& position, const Quaternion& rotation,
			UINT64 layer = BS_ALL_LAYERS) override;

		void setFlag(PhysicsFlags flags, bool enabled) override;

		Vector3 getGravity() const override;
		void setGravity(const Vector3& gravity) override;

		float getMaxTesselationEdgeLength() const override { return mTesselationLength; }
		void setMaxTesselationEdgeLength(float length) override;

		UINT32 addBroadPhaseRegion(const AABox& region) override;
		void removeBroadPhaseRegion(UINT32 regionId) override;
		void clearBroadPhaseRegions() override;

		/** @copydoc Physics::_boxOverlap */
		Vector<Collider*> _boxOverlap(const AABox& box, const Quaternion& rotation,
			UINT64 layer = BS_ALL_LAYERS) override;

		/** @copydoc Physics::_sphereOverlap */
		Vector<Collider*> _sphereOverlap(const Sphere& sphere, UINT64 layer = BS_ALL_LAYERS) override;

		/** @copydoc Physics::_capsuleOverlap */
		Vector<Collider*> _capsuleOverlap(const Capsule& capsule, const Quaternion& rotation,
			UINT64 layer = BS_ALL_LAYERS) override;

		/** @copydoc Physics::_convexOverlap */
		Vector<Collider*> _convexOverlap(const HPhysicsMesh& mesh, const Vector3& position,
			const Quaternion& rotation, UINT64 layer = BS_ALL_LAYERS) override;

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

		// Scene query helpers
		inline bool sweep(const physx::PxGeometry& geometry, const physx::PxTransform& tfrm, const Vector3& unitDir,
			PhysicsQueryHit& hit, UINT64 layer, float maxDist) const;
		inline Vector<PhysicsQueryHit> sweepAll(const physx::PxGeometry& geometry, const physx::PxTransform& tfrm, 
			const Vector3& unitDir, UINT64 layer, float maxDist) const;
		inline bool sweepAny(const physx::PxGeometry& geometry, const physx::PxTransform& tfrm, const Vector3& unitDir,
			UINT64 layer, float maxDist) const;
		inline Vector<Collider*> overlap(const physx::PxGeometry& geometry, const physx::PxTransform& tfrm, UINT64 layer) const;
		inline bool overlapAny(const physx::PxGeometry& geometry, const physx::PxTransform& tfrm, UINT64 layer) const;

		float mSimulationStep = 1.0f/60.0f;
		float mLastSimulationTime = 0.0f;
		float mTesselationLength = 3.0f;
		UINT32 mNextRegionIdx = 1;

		Vector<TriggerEvent> mTriggerEvents;
		Vector<ContactEvent> mContactEvents;
		Vector<JointBreakEvent> mJointBreakEvents;
		UnorderedMap<UINT32, UINT32> mBroadPhaseRegionHandles;

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
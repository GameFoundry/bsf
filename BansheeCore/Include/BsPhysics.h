//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"
#include "BsVector3.h"
#include "BsQuaternion.h"

namespace BansheeEngine
{
	struct PHYSICS_INIT_DESC;

	/** Flags for controlling physics behaviour globally. */
	enum class PhysicsFlag
	{
		/** 
		 * Automatically recovers character controllers that are interpenetrating geometry. This can happen if a controller
		 * is spawned or teleported into geometry, its size/rotation is changed so it penetrates geometry, or simply
		 * because of numerical imprecision.
		 */
		CCT_OverlapRecovery = 1<<0,
		/**
		 * Performs more accurate sweeps when moving the character controller, making it less likely to interpenetrate
		 * geometry. When overlap recovery is turned on you can consider turning this off as it can compensate for the
		 * less precise sweeps.
		 */
		CCT_PreciseSweeps = 1<<1,
		/**
		 * Large triangles can cause problems with character controller collision. If this option is enabled the triangles
		 * larger than a certain size will be automatically tesselated into smaller triangles, in order to help with
		 * precision.
		 *
		 * @see Physics::getMaxTesselationEdgeLength
		 */
		CCT_Tesselation = 1<<2
	};

	/** @copydoc CharacterCollisionFlag */
	typedef Flags<PhysicsFlag> PhysicsFlags;
	BS_FLAGS_OPERATORS(PhysicsFlag)

	class BS_CORE_EXPORT Physics : public Module<Physics>
	{
	public:
		Physics(const PHYSICS_INIT_DESC& init);
		virtual ~Physics() { }

		virtual void update() = 0;

		virtual SPtr<PhysicsMaterial> createMaterial(float staticFriction, float dynamicFriction, float restitution) = 0;
		virtual SPtr<PhysicsMesh> createMesh(const MeshDataPtr& meshData, PhysicsMeshType type) = 0;
		virtual SPtr<Rigidbody> createRigidbody(const HSceneObject& linkedSO) = 0;

		virtual SPtr<BoxCollider> createBoxCollider(const Vector3& extents, const Vector3& position, 
			const Quaternion& rotation) = 0;
		virtual SPtr<SphereCollider> createSphereCollider(float radius,
			const Vector3& position, const Quaternion& rotation) = 0;
		virtual SPtr<PlaneCollider> createPlaneCollider(const Vector3& position, const Quaternion& rotation) = 0;
		virtual SPtr<CapsuleCollider> createCapsuleCollider(float radius, float halfHeight,
			const Vector3& position, const Quaternion& rotation) = 0;
		virtual SPtr<MeshCollider> createMeshCollider(const Vector3& position, const Quaternion& rotation) = 0;

		virtual SPtr<FixedJoint> createFixedJoint() = 0;
		virtual SPtr<DistanceJoint> createDistanceJoint() = 0;
		virtual SPtr<HingeJoint> createHingeJoint() = 0;
		virtual SPtr<SphericalJoint> createSphericalJoint() = 0;
		virtual SPtr<SliderJoint> createSliderJoint() = 0;
		virtual SPtr<D6Joint> createD6Joint() = 0;

		/** @copydoc CharacterController::create */
		virtual SPtr<CharacterController> createCharacterController(const CHAR_CONTROLLER_DESC& desc) = 0;

		virtual bool hasFlag(PhysicsFlags flag) const { return mFlags & flag; }
		virtual void setFlag(PhysicsFlags flag, bool enabled) { if (enabled) mFlags |= flag; else mFlags &= ~flag; }

		virtual Vector3 getGravity() const = 0;
		virtual void setGravity(const Vector3& gravity) = 0;

		virtual UINT32 addBroadPhaseRegion(const AABox& region) = 0;
		virtual void removeBroadPhaseRegion(UINT32 handle) = 0;
		virtual void clearBroadPhaseRegions() = 0;

		/** 
		 * Returns a maximum edge length before a triangle is tesselated. 
		 *
		 * @see PhysicsFlags::CCT_Tesselation
		 */
		virtual float getMaxTesselationEdgeLength() const = 0;

		/** 
		 * Sets a maximum edge length before a triangle is tesselated. 
		 *
		 * @see PhysicsFlags::CCT_Tesselation
		 */
		virtual void setMaxTesselationEdgeLength(float length) = 0;

		void toggleCollision(UINT64 groupA, UINT64 groupB, bool enabled);
		bool isCollisionEnabled(UINT64 groupA, UINT64 groupB) const;

		bool _isUpdateInProgress() const { return mUpdateInProgress; }

		static const UINT64 CollisionMapSize = 64;
	protected:
		friend class Rigidbody;

		void registerRigidbody(Rigidbody* body, UINT32 priority);
		void unregisterRigidbody(UINT32 id, UINT32 priority);
		void updatePriority(UINT32 id, UINT32 oldPriority, UINT32 newPriority);

		mutable Mutex mMutex;
		bool mCollisionMap[CollisionMapSize][CollisionMapSize];

		bool mUpdateInProgress = false;
		PhysicsFlags mFlags;
		Vector<Vector<Rigidbody*>> mRigidbodies; // TODO: Unused for now, but keeping it here just in case I change the design. Remove later.

		const static UINT32 MAX_PRIORITY = 128;
	};

	/** Provides easier access to Physics. */
	BS_CORE_EXPORT Physics& gPhysics();

	struct PHYSICS_INIT_DESC
	{
		float typicalLength = 1.0f;
		float typicalSpeed = 9.81f;
		Vector3 gravity = Vector3(0.0f, -9.81f, 0.0f);
		bool initCooking = true; // TODO: Disable this for Game build
		float timeStep = 1.0f / 60.0f;
		PhysicsFlags flags = PhysicsFlag::CCT_OverlapRecovery | PhysicsFlag::CCT_PreciseSweeps;
	};
}
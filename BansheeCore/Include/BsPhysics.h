//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsPhysicsCommon.h"
#include "BsModule.h"
#include "BsVector3.h"
#include "BsVector2.h"
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

		/******************************************************************************************************************/
		/************************************************* CREATION *******************************************************/
		/******************************************************************************************************************/

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

		/******************************************************************************************************************/
		/************************************************* QUERIES ********************************************************/
		/******************************************************************************************************************/

		/**
		 * Casts a ray into the scene and returns the closest found hit, if any.
		 * 
		 * @param[in]	ray		Ray to cast into the scene.
		 * @param[out]	hit		Information recorded about a hit. Only valid if method returns true.
		 * @param[in]	layer	Layers to consider for the query. This allows you to ignore certain groups of objects.
		 * @param[in]	max		Maximum distance at which to perform the query. Hits past this distance will not be
		 *						detected.
		 * @return				True if something was hit, false otherwise.
		 */
		virtual bool rayCast(const Ray& ray, PhysicsQueryHit& hit, UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) const;

		/**
		 * Casts a ray into the scene and returns the closest found hit, if any.
		 * 
		 * @param[in]	origin		Origin of the ray to cast into the scene.
		 * @param[in]	unitDir		Unit direction of the ray to cast into the scene.
		 * @param[out]	hit			Information recorded about a hit. Only valid if method returns true.
		 * @param[in]	layer		Layers to consider for the query. This allows you to ignore certain groups of objects.
		 * @param[in]	max			Maximum distance at which to perform the query. Hits past this distance will not be
		 *							detected.
		 * @return					True if something was hit, false otherwise.
		 */
		virtual bool rayCast(const Vector3& origin, const Vector3& unitDir, PhysicsQueryHit& hit,
			UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) const = 0;

		/**
		 * Performs a sweep into the scene using a box and returns the closest found hit, if any.
		 * 
		 * @param[in]	box			Box to sweep through the scene.
		 * @param[in]	rotation	Orientation of the box.
		 * @param[in]	unitDir		Unit direction towards which to perform the sweep.
		 * @param[out]	hit			Information recorded about a hit. Only valid if method returns true.
		 * @param[in]	layer		Layers to consider for the query. This allows you to ignore certain groups of objects.
		 * @param[in]	max			Maximum distance at which to perform the query. Hits past this distance will not be
		 *							detected.
		 * @return					True if something was hit, false otherwise.
		 */
		virtual bool boxCast(const AABox& box, const Quaternion& rotation, const Vector3& unitDir, PhysicsQueryHit& hit,
			UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) const = 0;

		/**
		 * Performs a sweep into the scene using a sphere and returns the closest found hit, if any.
		 * 
		 * @param[in]	sphere		Sphere to sweep through the scene.
		 * @param[in]	unitDir		Unit direction towards which to perform the sweep.
		 * @param[out]	hit			Information recorded about a hit. Only valid if method returns true.
		 * @param[in]	layer		Layers to consider for the query. This allows you to ignore certain groups of objects.
		 * @param[in]	max			Maximum distance at which to perform the query. Hits past this distance will not be
		 *							detected.
		 * @return					True if something was hit, false otherwise.
		 */
		virtual bool sphereCast(const Sphere& sphere, const Vector3& unitDir, PhysicsQueryHit& hit,
			UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) const = 0;

		/**
		 * Performs a sweep into the scene using a capsule and returns the closest found hit, if any.
		 * 
		 * @param[in]	capsule		Capsule to sweep through the scene.
		 * @param[in]	rotation	Orientation of the capsule.
		 * @param[in]	unitDir		Unit direction towards which to perform the sweep.
		 * @param[out]	hit			Information recorded about a hit. Only valid if method returns true.
		 * @param[in]	layer		Layers to consider for the query. This allows you to ignore certain groups of objects.
		 * @param[in]	max			Maximum distance at which to perform the query. Hits past this distance will not be
		 *							detected.
		 * @return					True if something was hit, false otherwise.
		 */
		virtual bool capsuleCast(const Capsule& capsule, const Quaternion& rotation, const Vector3& unitDir,
			PhysicsQueryHit& hit, UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) const = 0;

		/**
		 * Performs a sweep into the scene using a convex mesh and returns the closest found hit, if any.
		 * 
		 * @param[in]	mesh		Mesh to sweep through the scene. Must be convex.
		 * @param[in]	position	Starting position of the mesh.
		 * @param[in]	rotation	Orientation of the mesh.
		 * @param[in]	unitDir		Unit direction towards which to perform the sweep.
		 * @param[out]	hit			Information recorded about a hit. Only valid if method returns true.
		 * @param[in]	layer		Layers to consider for the query. This allows you to ignore certain groups of objects.
		 * @param[in]	max			Maximum distance at which to perform the query. Hits past this distance will not be
		 *							detected.
		 * @return					True if something was hit, false otherwise.
		 */
		virtual bool convexCast(const HPhysicsMesh& mesh, const Vector3& position, const Quaternion& rotation,
			const Vector3& unitDir, PhysicsQueryHit& hit, UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) const = 0;

		/**
		 * Casts a ray into the scene and returns all found hits.
		 * 
		 * @param[in]	ray		Ray to cast into the scene.
		 * @param[in]	layer	Layers to consider for the query. This allows you to ignore certain groups of objects.
		 * @param[in]	max		Maximum distance at which to perform the query. Hits past this distance will not be
		 *						detected.
		 * @return				List of all detected hits.
		 */
		virtual Vector<PhysicsQueryHit> rayCastAll(const Ray& ray, UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) const;

		/**
		 * Casts a ray into the scene and returns all found hits.
		 * 
		 * @param[in]	origin		Origin of the ray to cast into the scene.
		 * @param[in]	unitDir		Unit direction of the ray to cast into the scene.
		 * @param[in]	layer		Layers to consider for the query. This allows you to ignore certain groups of objects.
		 * @param[in]	max			Maximum distance at which to perform the query. Hits past this distance will not be
		 *							detected.
		 * @return					List of all detected hits.
		 */
		virtual Vector<PhysicsQueryHit> rayCastAll(const Vector3& origin, const Vector3& unitDir,
			UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) const = 0;

		/**
		 * Performs a sweep into the scene using a box and returns all found hits.
		 * 
		 * @param[in]	box			Box to sweep through the scene.
		 * @param[in]	rotation	Orientation of the box.
		 * @param[in]	unitDir		Unit direction towards which to perform the sweep.
		 * @param[in]	layer		Layers to consider for the query. This allows you to ignore certain groups of objects.
		 * @param[in]	max			Maximum distance at which to perform the query. Hits past this distance will not be
		 *							detected.
		 * @return					List of all detected hits.
		 */
		virtual Vector<PhysicsQueryHit> boxCastAll(const AABox& box, const Quaternion& rotation, 
			const Vector3& unitDir, UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) const = 0;

		/**
		 * Performs a sweep into the scene using a sphere and returns all found hits.
		 * 
		 * @param[in]	sphere		Sphere to sweep through the scene.
		 * @param[in]	unitDir		Unit direction towards which to perform the sweep.
		 * @param[in]	layer		Layers to consider for the query. This allows you to ignore certain groups of objects.
		 * @param[in]	max			Maximum distance at which to perform the query. Hits past this distance will not be
		 *							detected.
		 * @return					List of all detected hits.
		 */
		virtual Vector<PhysicsQueryHit> sphereCastAll(const Sphere& sphere, const Vector3& unitDir,
			UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) const = 0;

		/**
		 * Performs a sweep into the scene using a capsule and returns all found hits.
		 * 
		 * @param[in]	capsule		Capsule to sweep through the scene.
		 * @param[in]	rotation	Orientation of the capsule.
		 * @param[in]	unitDir		Unit direction towards which to perform the sweep.
		 * @param[in]	layer		Layers to consider for the query. This allows you to ignore certain groups of objects.
		 * @param[in]	max			Maximum distance at which to perform the query. Hits past this distance will not be
		 *							detected.
		 * @return					List of all detected hits.
		 */
		virtual Vector<PhysicsQueryHit> capsuleCastAll(const Capsule& capsule, const Quaternion& rotation, 
			const Vector3& unitDir, UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) const = 0;

		/**
		 * Performs a sweep into the scene using a convex mesh and returns all found hits.
		 * 
		 * @param[in]	mesh		Mesh to sweep through the scene. Must be convex.
		 * @param[in]	position	Starting position of the mesh.
		 * @param[in]	rotation	Orientation of the mesh.
		 * @param[in]	unitDir		Unit direction towards which to perform the sweep.
		 * @param[in]	layer		Layers to consider for the query. This allows you to ignore certain groups of objects.
		 * @param[in]	max			Maximum distance at which to perform the query. Hits past this distance will not be
		 *							detected.
		 * @return					List of all detected hits.
		 */
		virtual Vector<PhysicsQueryHit> convexCastAll(const HPhysicsMesh& mesh, const Vector3& position, 
			const Quaternion& rotation, const Vector3& unitDir, UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) const = 0;

		/**
		 * Casts a ray into the scene and checks if it has hit anything. This can be significantly more efficient than other
		 * types of cast* calls.
		 * 
		 * @param[in]	ray		Ray to cast into the scene.
		 * @param[in]	layer	Layers to consider for the query. This allows you to ignore certain groups of objects.
		 * @param[in]	max		Maximum distance at which to perform the query. Hits past this distance will not be
		 *						detected.
		 * @return				True if something was hit, false otherwise.
		 */
		virtual bool rayCastAny(const Ray& ray, UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) const;

		/**
		 * Casts a ray into the scene and checks if it has hit anything. This can be significantly more efficient than other
		 * types of cast* calls.
		 * 
		 * @param[in]	origin		Origin of the ray to cast into the scene.
		 * @param[in]	unitDir		Unit direction of the ray to cast into the scene.
		 * @param[in]	layer		Layers to consider for the query. This allows you to ignore certain groups of objects.
		 * @param[in]	max			Maximum distance at which to perform the query. Hits past this distance will not be
		 *							detected.
		 * @return					True if something was hit, false otherwise.
		 */
		virtual bool rayCastAny(const Vector3& origin, const Vector3& unitDir,
			UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) const = 0;

		/**
		 * Performs a sweep into the scene using a box and checks if it has hit anything. This can be significantly more
		 * efficient than other types of cast* calls.
		 * 
		 * @param[in]	box			Box to sweep through the scene.
		 * @param[in]	rotation	Orientation of the box.
		 * @param[in]	unitDir		Unit direction towards which to perform the sweep.
		 * @param[in]	layer		Layers to consider for the query. This allows you to ignore certain groups of objects.
		 * @param[in]	max			Maximum distance at which to perform the query. Hits past this distance will not be
		 *							detected.
		 * @return					True if something was hit, false otherwise.
		 */
		virtual bool boxCastAny(const AABox& box, const Quaternion& rotation, const Vector3& unitDir,
			UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) const = 0;

		/**
		 * Performs a sweep into the scene using a sphere and checks if it has hit anything. This can be significantly more
		 * efficient than other types of cast* calls.
		 * 
		 * @param[in]	sphere		Sphere to sweep through the scene.
		 * @param[in]	unitDir		Unit direction towards which to perform the sweep.
		 * @param[in]	layer		Layers to consider for the query. This allows you to ignore certain groups of objects.
		 * @param[in]	max			Maximum distance at which to perform the query. Hits past this distance will not be
		 *							detected.
		 * @return					True if something was hit, false otherwise.
		 */
		virtual bool sphereCastAny(const Sphere& sphere, const Vector3& unitDir,
			UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) const = 0;

		/**
		 * Performs a sweep into the scene using a capsule and checks if it has hit anything. This can be significantly more
		 * efficient than other types of cast* calls.
		 * 
		 * @param[in]	capsule		Capsule to sweep through the scene.
		 * @param[in]	rotation	Orientation of the capsule.
		 * @param[in]	unitDir		Unit direction towards which to perform the sweep.
		 * @param[in]	layer		Layers to consider for the query. This allows you to ignore certain groups of objects.
		 * @param[in]	max			Maximum distance at which to perform the query. Hits past this distance will not be
		 *							detected.
		 * @return					True if something was hit, false otherwise.
		 */
		virtual bool capsuleCastAny(const Capsule& capsule, const Quaternion& rotation, const Vector3& unitDir,
			UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) const = 0;

		/**
		 * Performs a sweep into the scene using a convex mesh and checks if it has hit anything. This can be significantly
		 * more efficient than other types of cast* calls.
		 * 
		 * @param[in]	mesh		Mesh to sweep through the scene. Must be convex.
		 * @param[in]	position	Starting position of the mesh.
		 * @param[in]	rotation	Orientation of the mesh.
		 * @param[in]	unitDir		Unit direction towards which to perform the sweep.
		 * @param[in]	layer		Layers to consider for the query. This allows you to ignore certain groups of objects.
		 * @param[in]	max			Maximum distance at which to perform the query. Hits past this distance will not be
		 *							detected.
		 * @return					True if something was hit, false otherwise.
		 */
		virtual bool convexCastAny(const HPhysicsMesh& mesh, const Vector3& position, const Quaternion& rotation,
			const Vector3& unitDir, UINT64 layer = BS_ALL_LAYERS, float max = FLT_MAX) const = 0;

		/**
		 * Returns a list of all colliders in the scene that overlap the provided box.
		 * 
		 * @param[in]	box			Box to check for overlap.
		 * @param[in]	rotation	Orientation of the box.
		 * @param[in]	layer		Layers to consider for the query. This allows you to ignore certain groups of objects.
		 * @return					List of all colliders that overlap the box.
		 */
		virtual Vector<HCollider> boxOverlap(const AABox& box, const Quaternion& rotation, 
			UINT64 layer = BS_ALL_LAYERS) const;

		/**
		 * Returns a list of all colliders in the scene that overlap the provided sphere.
		 * 
		 * @param[in]	Sphere		Sphere to check for overlap.
		 * @param[in]	layer		Layers to consider for the query. This allows you to ignore certain groups of objects.
		 * @return					List of all colliders that overlap the sphere.
		 */
		virtual Vector<HCollider> sphereOverlap(const Sphere& sphere, UINT64 layer = BS_ALL_LAYERS) const;

		/**
		 * Returns a list of all colliders in the scene that overlap the provided capsule.
		 * 
		 * @param[in]	capsule		Capsule to check for overlap.
		 * @param[in]	rotation	Orientation of the capsule.
		 * @param[in]	layer		Layers to consider for the query. This allows you to ignore certain groups of objects.
		 * @return					List of all colliders that overlap the capsule.
		 */
		virtual Vector<HCollider> capsuleOverlap(const Capsule& capsule, const Quaternion& rotation,
			UINT64 layer = BS_ALL_LAYERS) const;

		/**
		 * Returns a list of all colliders in the scene that overlap the provided convex mesh.
		 * 
		 * @param[in]	mesh		Mesh to check for overlap. Must be convex.
		 * @param[in]	position	Position of the mesh.
		 * @param[in]	rotation	Orientation of the mesh.
		 * @param[in]	layer		Layers to consider for the query. This allows you to ignore certain groups of objects.
		 * @return					List of all colliders that overlap the mesh.
		 */
		virtual Vector<HCollider> convexOverlap(const HPhysicsMesh& mesh, const Vector3& position, 
			const Quaternion& rotation, UINT64 layer = BS_ALL_LAYERS) const;

		/**
		 * Checks if the provided box overlaps any other collider in the scene.
		 * 
		 * @param[in]	box			Box to check for overlap.
		 * @param[in]	rotation	Orientation of the box.
		 * @param[in]	layer		Layers to consider for the query. This allows you to ignore certain groups of objects.
		 * @return					True if there is overlap with another object, false otherwise.
		 */
		virtual bool boxOverlapAny(const AABox& box, const Quaternion& rotation, UINT64 layer = BS_ALL_LAYERS) const = 0;

		/**
		 * Checks if the provided sphere overlaps any other collider in the scene.
		 * 
		 * @param[in]	sphere		Sphere to check for overlap.
		 * @param[in]	layer		Layers to consider for the query. This allows you to ignore certain groups of objects.
		 * @return					True if there is overlap with another object, false otherwise.
		 */
		virtual bool sphereOverlapAny(const Sphere& sphere, UINT64 layer = BS_ALL_LAYERS) const = 0;

		/**
		 * Checks if the provided capsule overlaps any other collider in the scene.
		 * 
		 * @param[in]	capsule		Capsule to check for overlap.
		 * @param[in]	rotation	Orientation of the capsule.
		 * @param[in]	layer		Layers to consider for the query. This allows you to ignore certain groups of objects.
		 * @return					True if there is overlap with another object, false otherwise.
		 */
		virtual bool capsuleOverlapAny(const Capsule& capsule, const Quaternion& rotation, 
			UINT64 layer = BS_ALL_LAYERS) const = 0;

		/**
		 * Checks if the provided convex mesh overlaps any other collider in the scene.
		 * 
		 * @param[in]	mesh		Mesh to check for overlap. Must be convex.
		 * @param[in]	position	Position of the mesh.
		 * @param[in]	rotation	Orientation of the mesh.
		 * @param[in]	layer		Layers to consider for the query. This allows you to ignore certain groups of objects.
		 * @return					True if there is overlap with another object, false otherwise.
		 */
		virtual bool convexOverlapAny(const HPhysicsMesh& mesh, const Vector3& position, const Quaternion& rotation,
			UINT64 layer = BS_ALL_LAYERS) const = 0;

		/******************************************************************************************************************/
		/************************************************* OPTIONS ********************************************************/
		/******************************************************************************************************************/

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

		/** @cond INTERNAL */

		/** @copydoc Physics::boxOverlap() */
		virtual Vector<Collider*> _boxOverlap(const AABox& box, const Quaternion& rotation,
			UINT64 layer = BS_ALL_LAYERS) const = 0;

		/** @copydoc Physics::sphereOverlap() */
		virtual Vector<Collider*> _sphereOverlap(const Sphere& sphere, UINT64 layer = BS_ALL_LAYERS) const = 0;

		/** @copydoc Physics::capsuleOverlap() */
		virtual Vector<Collider*> _capsuleOverlap(const Capsule& capsule, const Quaternion& rotation,
			UINT64 layer = BS_ALL_LAYERS) const = 0;

		/** @copydoc Physics::convexOverlap() */
		virtual Vector<Collider*> _convexOverlap(const HPhysicsMesh& mesh, const Vector3& position,
			const Quaternion& rotation, UINT64 layer = BS_ALL_LAYERS) const = 0;

		/** 
		 * Checks does the ray hit the provided collider. 
		 *
		 * @param[in]	origin		Origin of the ray to check.
		 * @param[in]	unitDir		Unit direction of the ray to check.
		 * @param[in]	collider	Collider to check for hit.
		 * @param[out]	hit			Information about the hit. Valid only if the method returns true.
		 * @param[in]	maxDist		Maximum distance from the ray origin to search for hits.
		 * @return					True if the ray has hit the collider.
		 */
		virtual bool _rayCast(const Vector3& origin, const Vector3& unitDir, const Collider& collider, PhysicsQueryHit& hit, 
			float maxDist = FLT_MAX) const = 0;

		bool _isUpdateInProgress() const { return mUpdateInProgress; }

		/** @endcond */

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
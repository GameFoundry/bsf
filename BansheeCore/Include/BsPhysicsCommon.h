//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsVector3.h"

namespace BansheeEngine
{
	/** Information about a single contact point during physics collision. */
	struct ContactPoint
	{
		Vector3 position; /**< Contact point in world space. */
		Vector3 normal; /**< Normal pointing from the second shape to the first shape. */
		/** Impulse applied to the objects to keep them from penetrating. Divide by simulation step to get the force. */
		float impulse;
		float separation; /**< Determines how far are the objects. Negative value denotes penetration. */
	};

	/** Information about a collision between two physics objects. */
	struct CollisionData
	{
		Collider* colliderRaw; /**< Collider that was hit. */

		/** 
		 * Component of the collider that was hit. Can be null if collider is not owned by a component, in which case
		 * use ::colliderRaw directly. 
		 */
		HCollider collider; 
		// Note: Not too happy this is heap allocated, use static allocator?
		Vector<ContactPoint> contactPoints; /**< Information about all the contact points for the hit */ 
	};

	/** Determines what parent, if any, owns a physics object. */
	enum class PhysicsOwnerType
	{
		None, /** No parent, object is used directly. */
		Component, /** Object is used by a C++ Component. */
		Script /** Object is used by the scripting system. */
	};

	/** Contains information about a parent for a physics object. */
	struct PhysicsObjectOwner
	{
		PhysicsOwnerType type = PhysicsOwnerType::None; /**< Type of owner. */
		void* ownerData = nullptr; /**< Data managed by the owner. */
	};
}
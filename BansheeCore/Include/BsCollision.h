//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsVector3.h"

namespace BansheeEngine
{
	struct ContactPoint
	{
		Vector3 position; /**< Contact point in world space. */
		Vector3 normal; /**< Normal pointing from the second shape to the first shape. */
		/** Impulse applied to the objects to keep them from penetrating. Divide by simulation step to get the force. */
		float impulse;
		float separation; /**< Determines how far are the objects. Negative value denotes penetration. */
	};

	struct CollisionData
	{
		Collider* collider;
		Vector<ContactPoint> contactPoints; // Note: Not too happy this is heap allocated, use static allocator?
	};
}
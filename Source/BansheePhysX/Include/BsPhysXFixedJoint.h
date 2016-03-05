//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPhysXPrerequisites.h"
#include "BsFixedJoint.h"
#include "PxPhysics.h"

namespace BansheeEngine
{
	/** @addtogroup PhysX
	 *  @{
	 */

	/** PhysX implementation of a FixedJoint. */
	class BS_PHYSX_EXPORT PhysXFixedJoint : public FixedJoint
	{
	public:
		PhysXFixedJoint(physx::PxPhysics* physx);
		~PhysXFixedJoint();
	};

	/** @} */
}
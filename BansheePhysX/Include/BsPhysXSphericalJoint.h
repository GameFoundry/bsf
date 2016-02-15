//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPhysXPrerequisites.h"
#include "BsSphericalJoint.h"
#include "PxPhysics.h"
#include "extensions\PxSphericalJoint.h"

namespace BansheeEngine
{
	class BS_PHYSX_EXPORT PhysXSphericalJoint : public SphericalJoint
	{
	public:
		PhysXSphericalJoint(physx::PxPhysics* physx);
		~PhysXSphericalJoint();

		LimitConeRange getLimit() const override;
		void setLimit(const LimitConeRange& limit) override;

		void setFlag(Flag flag, bool enabled) override;
		bool hasFlag(Flag flag) override;

	private:
		inline physx::PxSphericalJoint* getInternal() const;
	};
}
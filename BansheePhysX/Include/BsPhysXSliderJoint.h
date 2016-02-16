//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPhysXPrerequisites.h"
#include "BsSliderJoint.h"
#include "PxPhysics.h"
#include "extensions\PxPrismaticJoint.h"

namespace BansheeEngine
{
	class BS_PHYSX_EXPORT PhysXSliderJoint : public SliderJoint
	{
	public:
		PhysXSliderJoint(physx::PxPhysics* physx);
		~PhysXSliderJoint();

		float getPosition() const override;
		float getSpeed() const override;

		LimitLinearRange getLimit() const override;
		void setLimit(const LimitLinearRange& limit) override;

		void setFlag(Flag flag, bool enabled) override;
		bool hasFlag(Flag flag) const override;

	private:
		inline physx::PxPrismaticJoint* getInternal() const;
	};
}
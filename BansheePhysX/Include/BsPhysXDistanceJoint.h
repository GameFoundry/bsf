//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPhysXPrerequisites.h"
#include "BsDistanceJoint.h"
#include "PxPhysics.h"
#include "extensions\PxDistanceJoint.h"

namespace BansheeEngine
{
	class BS_PHYSX_EXPORT PhysXDistanceJoint : public DistanceJoint
	{
	public:
		PhysXDistanceJoint(physx::PxPhysics* physx);
		~PhysXDistanceJoint();

		float getDistance() const override;

		float getMinDistance() const override;
		void setMinDistance(float value) override;

		float getMaxDistance() const override;
		void setMaxDistance(float value) override;

		float getTolerance() const override;
		void setTolerance(float value) override;

		Spring getSpring() const override;
		void setSpring(const Spring& value) override;

		void setFlag(Flag flag, bool enabled) override;
		bool hasFlag(Flag flag) const override;

	private:
		inline physx::PxDistanceJoint* getInternal() const;
	};
}
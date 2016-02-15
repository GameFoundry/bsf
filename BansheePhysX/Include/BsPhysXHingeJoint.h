//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPhysXPrerequisites.h"
#include "BsHingeJoint.h"
#include "PxPhysics.h"
#include "extensions\PxRevoluteJoint.h"

namespace BansheeEngine
{
	class BS_PHYSX_EXPORT PhysXHingeJoint : public HingeJoint
	{
	public:
		PhysXHingeJoint(physx::PxPhysics* physx);
		~PhysXHingeJoint();

		Radian getAngle() const override;
		float getSpeed() const override;

		LimitAngular getLimit() const override;
		void setLimit(const LimitAngular& limit) const override;

		Drive getDrive() const override;
		void setDrive(const Drive& drive) const override;

		void setFlag(Flag flag, bool enabled) override;
		bool hasFlag(Flag flag) override;

	private:
		inline physx::PxRevoluteJoint* getInternal() const;
	};
}
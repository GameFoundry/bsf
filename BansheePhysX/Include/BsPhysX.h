#pragma once

#include "BsPhysXPrerequisites.h"
#include "BsPhysics.h"
#include "PxPhysics.h"
#include "foundation/Px.h"
#include "cooking/PxCooking.h"

namespace BansheeEngine
{
	class PhysX : public Physics
	{
	public:
		PhysX();
		~PhysX();

		void update() override;

	private:
		float mSimulationStep = 1.0f/60.0f;
		float mLastSimulationTime = 0.0f;

		physx::PxFoundation* mFoundation = nullptr;
		physx::PxPhysics* mPhysics = nullptr;
		physx::PxCooking* mCooking = nullptr;
		physx::PxScene* mScene = nullptr;
	};
}
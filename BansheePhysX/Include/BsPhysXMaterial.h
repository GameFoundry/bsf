//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPhysXPrerequisites.h"
#include "BsPhysicsMaterial.h"
#include "PxMaterial.h"

namespace BansheeEngine
{
	class PhysXMaterial : public PhysicsMaterial
	{
	public:
		PhysXMaterial(physx::PxPhysics* physx, float staFric, float dynFriction, float restitution);
		~PhysXMaterial();

		void setStaticFriction(float value) override;
		float getStaticFriction() const override;

		void setDynamicFriction(float value) override;
		float getDynamicFriction() const override;

		void setRestitutionCoefficient(float value) override;
		float getRestitutionCoefficient() const override;

		physx::PxMaterial* _getInternal() const { return mInternal; }

	private:
		physx::PxMaterial* mInternal;
	};
}
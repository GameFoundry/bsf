#include "BsPhysicsMaterial.h"
#include "BsPhysicsMaterialRTTI.h"
#include "BsResources.h"
#include "BsPhysics.h"

namespace BansheeEngine
{
	HPhysicsMaterial PhysicsMaterial::create(float staticFriction, float dynamicFriction, float restitution)
	{
		PhysicsMaterialPtr newMaterial = _createPtr(staticFriction, dynamicFriction, restitution);

		return static_resource_cast<PhysicsMaterial>(gResources()._createResourceHandle(newMaterial));
	}

	PhysicsMaterialPtr PhysicsMaterial::_createPtr(float staticFriction, float dynamicFriction, float restitution)
	{
		PhysicsMaterialPtr newMaterial = gPhysics().createMaterial(staticFriction, dynamicFriction, restitution);
		newMaterial->_setThisPtr(newMaterial);
		newMaterial->initialize();

		return newMaterial;
	}

	RTTITypeBase* PhysicsMaterial::getRTTIStatic()
	{
		return PhysicsMaterialRTTI::instance();
	}

	RTTITypeBase* PhysicsMaterial::getRTTI() const
	{
		return getRTTIStatic();
	}
}
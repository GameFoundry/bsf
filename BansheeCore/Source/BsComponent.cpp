//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsComponent.h"
#include "BsSceneObject.h"
#include "BsComponentRTTI.h"

namespace BansheeEngine
{
	Component::Component(const HSceneObject& parent)
		:mParent(parent)
	{
		setName("Component");
	}

	Component::~Component()
	{

	}

	void Component::destroy()
	{
		SO()->destroyComponent(this);
	}

	RTTITypeBase* Component::getRTTIStatic()
	{
		return ComponentRTTI::instance();
	}

	RTTITypeBase* Component::getRTTI() const
	{
		return Component::getRTTIStatic();
	}
}
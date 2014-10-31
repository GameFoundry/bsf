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

	void Component::destroy(bool immediate)
	{
		SO()->destroyComponent(this, immediate);
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
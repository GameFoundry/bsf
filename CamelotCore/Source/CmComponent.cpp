#include "CmComponent.h"
#include "CmSceneObject.h"
#include "CmComponentRTTI.h"

namespace CamelotFramework
{
	Component::Component(const HSceneObject& parent)
		:mParent(parent)
	{

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
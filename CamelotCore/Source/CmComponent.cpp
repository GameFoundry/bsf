#include "CmComponent.h"
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

	RTTITypeBase* Component::getRTTIStatic()
	{
		return ComponentRTTI::instance();
	}

	RTTITypeBase* Component::getRTTI() const
	{
		return Component::getRTTIStatic();
	}
}
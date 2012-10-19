#include "CmComponent.h"
#include "CmComponentRTTI.h"

namespace CamelotEngine
{
	Component::Component(GameObjectPtr parent)
		:mParent(parent), mIsDestroyed(false)
	{

	}

	Component::~Component()
	{
		if(!mIsDestroyed)
			destroy();
	}

	void Component::destroy()
	{
		mIsDestroyed = true;
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
#include "CmComponent.h"

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
		mParent = nullptr;
	}
}
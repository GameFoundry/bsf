#include "BsIResourceListener.h"
#include "BsResourceListenerManager.h"

namespace BansheeEngine
{
	IResourceListener::IResourceListener()
	{
		ResourceListenerManager::instance().registerListener(this);
	}

	IResourceListener::~IResourceListener()
	{
		ResourceListenerManager::instance().unregisterListener(this);
	}

	void IResourceListener::markListenerResourcesDirty()
	{
		ResourceListenerManager::instance().markListenerDirty(this);
	}
}
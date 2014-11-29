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

	void IResourceListener::markResourcesDirty()
	{
		ResourceListenerManager::instance().markListenerDirty(this);
	}
}
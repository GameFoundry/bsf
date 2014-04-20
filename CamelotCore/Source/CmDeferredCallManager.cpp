#include "CmDeferredCallManager.h"

namespace BansheeEngine
{
	DeferredCallManager::DeferredCallManager()
	{

	}

	void DeferredCallManager::queueDeferredCall(std::function<void()> func)
	{
		mCallbacks.push_back(func);
	}

	void DeferredCallManager::update()
	{
		while(!mCallbacks.empty())
		{
			// Copy because callbacks can be queued within callbacks
			Vector<std::function<void()>>::type callbackCopy = mCallbacks;
			mCallbacks.clear();

			for(auto& call : callbackCopy)
			{
				call();
			}
		}
	}

	// Declared in CmPrerequisites.h
	void deferredCall(std::function<void()> callback)
	{
		DeferredCallManager::instance().queueDeferredCall(callback);
	}
}
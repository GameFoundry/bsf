//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsDeferredCallManager.h"

namespace BansheeEngine
{
	DeferredCallManager::DeferredCallManager()
	{

	}

	void DeferredCallManager::queueDeferredCall(std::function<void()> func)
	{
		mCallbacks.push_back(func);
	}

	void DeferredCallManager::_update()
	{
		while(!mCallbacks.empty())
		{
			// Copy because callbacks can be queued within callbacks
			Vector<std::function<void()>> callbackCopy = mCallbacks;
			mCallbacks.clear();

			for(auto& call : callbackCopy)
			{
				call();
			}
		}
	}

	// Declared in BsPrerequisites.h
	void deferredCall(std::function<void()> callback)
	{
		DeferredCallManager::instance().queueDeferredCall(callback);
	}
}
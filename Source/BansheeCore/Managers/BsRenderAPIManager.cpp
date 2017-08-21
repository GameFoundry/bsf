//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "Managers/BsRenderAPIManager.h"
#include "Error/BsException.h"
#include "RenderAPI/BsRenderAPI.h"
#include "Utility/BsDynLib.h"
#include "Utility/BsDynLibManager.h"

namespace bs
{
	RenderAPIManager::RenderAPIManager()
		:mRenderAPIInitialized(false)
	{ }

	RenderAPIManager::~RenderAPIManager()
	{
		if(mRenderAPIInitialized)
		{
			ct::RenderAPI::instance().destroy();
			ct::RenderAPI::shutDown();
		}
	}

	SPtr<RenderWindow> RenderAPIManager::initialize(const String& pluginFilename, RENDER_WINDOW_DESC& primaryWindowDesc)
	{
		if(mRenderAPIInitialized)
			return nullptr;

		DynLib* loadedLibrary = gDynLibManager().load(pluginFilename);
		const char* name = "";

		if(loadedLibrary != nullptr)
		{
			typedef const char* (*GetPluginNameFunc)();

			GetPluginNameFunc getPluginNameFunc = (GetPluginNameFunc)loadedLibrary->getSymbol("getPluginName");
			name = getPluginNameFunc();
		}

		for(auto iter = mAvailableFactories.begin(); iter != mAvailableFactories.end(); ++iter)
		{
			if(strcmp((*iter)->name(), name) == 0)
			{
				(*iter)->create();		
				mRenderAPIInitialized = true;
				return ct::RenderAPI::instance().initialize(primaryWindowDesc);
			}
		}

		return nullptr;
	}

	void RenderAPIManager::registerFactory(SPtr<RenderAPIFactory> factory)
	{
		assert(factory != nullptr);

		mAvailableFactories.push_back(factory);
	}
}
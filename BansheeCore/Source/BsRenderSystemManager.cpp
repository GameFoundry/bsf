#include "BsRenderSystemManager.h"
#include "BsException.h"
#include "BsRenderSystem.h"
#include "BsDynLib.h"
#include "BsDynLibManager.h"

namespace BansheeEngine
{
	RenderSystemManager::RenderSystemManager()
		:mRenderSystemInitialized(false)
	{ }

	RenderSystemManager::~RenderSystemManager()
	{
		if(mRenderSystemInitialized)
		{
			RenderSystem::instance().destroy();
			RenderSystem::shutDown();
		}
	}

	RenderWindowPtr RenderSystemManager::initialize(const String& pluginFilename, RENDER_WINDOW_DESC& primaryWindowDesc)
	{
		if(mRenderSystemInitialized)
			return nullptr;

		DynLib* loadedLibrary = gDynLibManager().load(pluginFilename);
		String name = "";

		if(loadedLibrary != nullptr)
		{
			typedef const String& (*GetPluginNameFunc)();

			GetPluginNameFunc getPluginNameFunc = (GetPluginNameFunc)loadedLibrary->getSymbol("getPluginName");
			name = getPluginNameFunc();
		}

		for(auto iter = mAvailableFactories.begin(); iter != mAvailableFactories.end(); ++iter)
		{
			if((*iter)->name() == name)
			{
				(*iter)->create();		
				mRenderSystemInitialized = true;
				return RenderSystem::instance().initialize(primaryWindowDesc);
			}
		}

		return nullptr;
	}

	void RenderSystemManager::registerRenderSystemFactory(RenderSystemFactoryPtr factory)
	{
		assert(factory != nullptr);

		mAvailableFactories.push_back(factory);
	}
}
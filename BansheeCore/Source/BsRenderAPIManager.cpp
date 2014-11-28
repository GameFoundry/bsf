#include "BsRenderAPIManager.h"
#include "BsException.h"
#include "BsRenderAPI.h"
#include "BsDynLib.h"
#include "BsDynLibManager.h"

namespace BansheeEngine
{
	RenderAPIManager::RenderAPIManager()
		:mRenderSystemInitialized(false)
	{ }

	RenderAPIManager::~RenderAPIManager()
	{
		if(mRenderSystemInitialized)
		{
			RenderAPICore::instance().destroy();
			RenderAPICore::shutDown();
		}
	}

	RenderWindowPtr RenderAPIManager::initialize(const String& pluginFilename, RENDER_WINDOW_DESC& primaryWindowDesc)
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
				return RenderAPICore::instance().initialize(primaryWindowDesc);
			}
		}

		return nullptr;
	}

	void RenderAPIManager::registerFactory(RenderAPIFactoryPtr factory)
	{
		assert(factory != nullptr);

		mAvailableFactories.push_back(factory);
	}
}
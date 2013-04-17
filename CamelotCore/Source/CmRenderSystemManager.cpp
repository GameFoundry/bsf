#include "CmRenderSystemManager.h"
#include "CmException.h"
#include "CmRenderSystem.h"
#include "CmDynLib.h"
#include "CmDynLibManager.h"

namespace CamelotFramework
{
	void RenderSystemManager::setActive(const String& pluginFilename)
	{
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
				RenderSystem::instance().initialize();
			}
		}
	}

	void RenderSystemManager::registerRenderSystemFactory(RenderSystemFactoryPtr factory)
	{
		assert(factory != nullptr);

		mAvailableFactories.push_back(factory);
	}
}
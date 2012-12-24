#include "CmRenderSystemManager.h"
#include "CmException.h"
#include "CmRenderSystem.h"
#include "CmDynLib.h"
#include "CmDynLibManager.h"

namespace CamelotEngine
{
	void RenderSystemManager::startUp(const String& pluginFilename)
	{
		DynLib* loadedLibrary = gDynLibManager().load(pluginFilename);
		String name = "";

		if(loadedLibrary != nullptr)
		{
			typedef const String& (*GetPluginNameFunc)();

			GetPluginNameFunc getPluginNameFunc = (GetPluginNameFunc)loadedLibrary->getSymbol("getPluginName");
			name = getPluginNameFunc();
		}

		for(auto iter = getAvailableFactories().begin(); iter != getAvailableFactories().end(); ++iter)
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

		getAvailableFactories().push_back(factory);
	}

	std::vector<RenderSystemFactoryPtr>& RenderSystemManager::getAvailableFactories()
	{
		static std::vector<RenderSystemFactoryPtr> availableFactories;
		return availableFactories;
	}

	RenderSystem* RenderSystemManager::getActive()
	{
		return RenderSystem::instancePtr();
	}
}
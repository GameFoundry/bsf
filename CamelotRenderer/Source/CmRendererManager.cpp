#include "CmRendererManager.h"
#include "CmException.h"
#include "CmRenderer.h"
#include "CmDynLib.h"
#include "CmDynLibManager.h"

namespace CamelotEngine
{
	RendererPtr RendererManager::mActiveRenderer;

	void RendererManager::initialize(const String& pluginFilename)
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
				RendererPtr newRenderSystem = (*iter)->create();
				if(newRenderSystem != nullptr)
				{
					mActiveRenderer = newRenderSystem;
				}				
			}
		}

		if(mActiveRenderer == nullptr)
		{
			CM_EXCEPT(InternalErrorException, 
				"Cannot initialize renderer. Renderer with the name '" + name + "' cannot be found.")
		}
	}

	void RendererManager::registerFactory(RendererFactoryPtr factory)
	{
		assert(factory != nullptr);

		getAvailableFactories().push_back(factory);
	}

	std::vector<RendererFactoryPtr>& RendererManager::getAvailableFactories()
	{
		static std::vector<RendererFactoryPtr> availableFactories;
		return availableFactories;
	}
}
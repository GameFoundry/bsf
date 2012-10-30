#include "CmRendererManager.h"
#include "CmRenderer.h"
#include "CmException.h"

namespace CamelotEngine
{
	RendererPtr RendererManager::mActiveRenderer;

	void RendererManager::setActive(const String& name)
	{
		for(auto iter = getAvailableFactories().begin(); iter != getAvailableFactories().end(); ++iter)
		{
			if((*iter)->name() == name)
			{
				RendererPtr newRenderer = (*iter)->create();
				if(newRenderer != nullptr)
				{
					mActiveRenderer = newRenderer;
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
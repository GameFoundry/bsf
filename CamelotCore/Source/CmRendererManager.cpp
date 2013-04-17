#include "CmRendererManager.h"
#include "CmRenderer.h"
#include "CmException.h"

namespace CamelotFramework
{
	void RendererManager::setActive(const String& name)
	{
		for(auto iter = mAvailableFactories.begin(); iter != mAvailableFactories.end(); ++iter)
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

	const String& RendererManager::getCoreRendererName()
	{
		static String name = "CoreRenderer";
		return name;
	}

	void RendererManager::registerFactory(RendererFactoryPtr factory)
	{
		assert(factory != nullptr);

		mAvailableFactories.push_back(factory);
	}
}
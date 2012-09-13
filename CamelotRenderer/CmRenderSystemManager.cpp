#include "CmRenderSystemManager.h"
#include "CmException.h"
#include "CmRenderSystem.h"

namespace CamelotEngine
{
	RenderSystemPtr RenderSystemManager::mActiveRenderSystem;

	void RenderSystemManager::initialize(const std::string& name)
	{
		assert(mActiveRenderSystem == nullptr); // Once loaded the render system is set for life

		for(auto iter = getAvailableFactories().begin(); iter != getAvailableFactories().end(); ++iter)
		{
			if((*iter)->name() == name)
			{
				RenderSystemPtr newRenderSystem = (*iter)->create();
				if(newRenderSystem != nullptr)
				{
					if(mActiveRenderSystem != nullptr)
						mActiveRenderSystem->shutdown();

					mActiveRenderSystem = newRenderSystem;
				}				
			}
		}

		if(mActiveRenderSystem == nullptr)
		{
			OGRE_EXCEPT(CamelotEngine::Exception::ERR_INTERNAL_ERROR, 
				"Cannot initialize render system. Renderer with the name '" + name + "' cannot be found.",
				"RenderSystemManager::initialize")
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
}
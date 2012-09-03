#include "CmRenderSystemManager.h"
#include "OgreException.h"
#include "OgreRenderSystem.h"

namespace CamelotEngine
{
	RenderSystemPtr RenderSystemManager::mActiveRenderSystem;

	void RenderSystemManager::initialize(const std::string& name)
	{
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
			OGRE_EXCEPT(Ogre::Exception::ERR_INTERNAL_ERROR, 
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
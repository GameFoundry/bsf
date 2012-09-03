#include "CmRenderSystemFactory.h"

namespace CamelotEngine
{
	RenderSystemPtr RenderSystemManager::create(const std::string& name)
	{
		for(auto iter = getAvailableFactories().begin(); iter != getAvailableFactories().end(); ++iter)
		{
			if((*iter)->name() == name)
				return (*iter)->create();
		}

		return nullptr;
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
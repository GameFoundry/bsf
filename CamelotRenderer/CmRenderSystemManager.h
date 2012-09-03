#pragma once

#include <string>
#include <vector>
#include <memory>
#include <assert.h>

#include "CmRenderSystemFactory.h"

namespace CamelotEngine
{
	typedef std::shared_ptr<RenderSystemFactory> RenderSystemFactoryPtr;

	class RenderSystemManager
	{
	public:
		static void initialize(const std::string& name);
		static Ogre::RenderSystem* getActive() { return mActiveRenderSystem.get(); }

		static void registerRenderSystemFactory(RenderSystemFactoryPtr factory);
	private:
		static std::vector<RenderSystemFactoryPtr>& getAvailableFactories();

		static RenderSystemPtr mActiveRenderSystem;
	};
}


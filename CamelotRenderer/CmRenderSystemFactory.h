#pragma once

#include <string>
#include <vector>
#include <memory>
#include <assert.h>

namespace Ogre
{
	class RenderSystem;
}

namespace CamelotEngine
{
	typedef std::shared_ptr<Ogre::RenderSystem> RenderSystemPtr;

	class RenderSystemFactory
	{
	public:
		virtual RenderSystemPtr create() = 0;
		virtual const std::string& name() const = 0;
	};

	typedef std::shared_ptr<RenderSystemFactory> RenderSystemFactoryPtr;

	class RenderSystemManager
	{
	public:
		static RenderSystemPtr create(const std::string& name);
		static void registerRenderSystemFactory(RenderSystemFactoryPtr factory);

	private:
		static std::vector<RenderSystemFactoryPtr>& getAvailableFactories();
	};
}
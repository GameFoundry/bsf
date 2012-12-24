#pragma once

#include "CmPrerequisites.h"
#include "CmRenderSystemFactory.h"

namespace CamelotEngine
{
	typedef std::shared_ptr<RenderSystemFactory> RenderSystemFactoryPtr;

	class CM_EXPORT RenderSystemManager
	{
	public:
		static void startUp(const String& name);

		static void registerRenderSystemFactory(RenderSystemFactoryPtr factory);
	private:
		static std::vector<RenderSystemFactoryPtr>& getAvailableFactories();
	};
}


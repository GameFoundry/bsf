#pragma once

#include "CmPrerequisites.h"
#include "CmRendererFactory.h"

namespace CamelotEngine
{
	class CM_EXPORT RendererManager
	{
	public:
		static void setActive(const String& name);
		static RendererPtr getActive() { return mActiveRenderer; }

		static void registerFactory(RendererFactoryPtr factory);
	private:
		static std::vector<RendererFactoryPtr>& getAvailableFactories();

		static RendererPtr mActiveRenderer;
	};
}


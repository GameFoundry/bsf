#pragma once

#include "CmPrerequisites.h"
#include "CmRendererFactory.h"
#include "CmModule.h"

namespace CamelotEngine
{
	class CM_EXPORT RendererManager : public Module<RendererManager>
	{
	public:
		void setActive(const String& name);
		RendererPtr getActive() { return mActiveRenderer; }

		void registerFactory(RendererFactoryPtr factory);
	private:
		std::vector<RendererFactoryPtr> mAvailableFactories;

		RendererPtr mActiveRenderer;
	};
}


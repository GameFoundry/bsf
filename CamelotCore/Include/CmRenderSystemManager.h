#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"
#include "CmRenderSystemFactory.h"

namespace CamelotEngine
{
	typedef std::shared_ptr<RenderSystemFactory> RenderSystemFactoryPtr;

	class CM_EXPORT RenderSystemManager : public Module<RenderSystemManager>
	{
	public:
		void setActive(const String& name);

		void registerRenderSystemFactory(RenderSystemFactoryPtr factory);
	private:
		std::vector<RenderSystemFactoryPtr> mAvailableFactories;
	};
}


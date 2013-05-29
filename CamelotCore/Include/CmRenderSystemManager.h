#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"
#include "CmRenderSystemFactory.h"

namespace CamelotFramework
{
	typedef std::shared_ptr<RenderSystemFactory> RenderSystemFactoryPtr;

	class CM_EXPORT RenderSystemManager : public Module<RenderSystemManager>
	{
	public:
		RenderWindowPtr initialize(const String& name, RENDER_WINDOW_DESC& primaryWindowDesc);

		void registerRenderSystemFactory(RenderSystemFactoryPtr factory);
	private:
		vector<RenderSystemFactoryPtr>::type mAvailableFactories;
	};
}


#include "CmForwardRendererPrerequisites.h"
#include "CmForwardRendererFactory.h"
#include "CmRendererManager.h"

namespace CamelotEngine
{
	extern "C" CM_FWDRND_EXPORT const String& getPluginName()
	{
		return SystemName;
	}

	extern "C" CM_FWDRND_EXPORT void loadPlugin()
	{
		RendererManager::registerFactory(RendererFactoryPtr(new ForwardRendererFactory()));
	}
}
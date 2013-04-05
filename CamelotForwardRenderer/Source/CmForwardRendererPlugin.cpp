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
		RendererManager::instance().registerFactory(
			RendererFactoryPtr(CM_NEW(ForwardRendererFactory, GenAlloc) ForwardRendererFactory(),
			&MemAllocDeleter<ForwardRendererFactory, GenAlloc>::deleter));
	}
}
#include "BsForwardRendererPrerequisites.h"
#include "BsForwardRendererFactory.h"
#include "CmRendererManager.h"

using namespace CamelotEngine;

namespace BansheeEngine
{
	extern "C" BS_FWDRND_EXPORT const String& getPluginName()
	{
		return SystemName;
	}

	extern "C" BS_FWDRND_EXPORT void* loadPlugin()
	{
		RendererManager::instance().registerFactory(
			RendererFactoryPtr(CM_NEW(ForwardRendererFactory, GenAlloc) ForwardRendererFactory(),
			&MemAllocDeleter<ForwardRendererFactory, GenAlloc>::deleter));

		return nullptr;
	}
}
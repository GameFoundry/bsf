#include "BsForwardRendererPrerequisites.h"
#include "BsForwardRendererFactory.h"
#include "CmRendererManager.h"

namespace BansheeEngine
{
	/**
	 * @brief	Returns a name of the plugin.
	 */
	extern "C" BS_FWDRND_EXPORT const String& getPluginName()
	{
		return SystemName;
	}

	/**
	 * @brief	Entry point to the plugin. Called by the engine when the plugin is loaded.
	 */
	extern "C" BS_FWDRND_EXPORT void* loadPlugin()
	{
		RendererManager::instance()._registerFactory(cm_shared_ptr<ForwardRendererFactory>());

		return nullptr;
	}
}
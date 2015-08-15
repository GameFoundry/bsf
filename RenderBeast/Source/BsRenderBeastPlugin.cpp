#include "BsRenderBeastPrerequisites.h"
#include "BsRenderBeastFactory.h"
#include "BsRendererManager.h"

namespace BansheeEngine
{
	/**
	 * @brief	Returns a name of the plugin.
	 */
	extern "C" BS_BSRND_EXPORT const String& getPluginName()
	{
		return SystemName;
	}

	/**
	 * @brief	Entry point to the plugin. Called by the engine when the plugin is loaded.
	 */
	extern "C" BS_BSRND_EXPORT void* loadPlugin()
	{
		RendererManager::instance()._registerFactory(bs_shared_ptr_new<RenderBeastFactory>());

		return nullptr;
	}
}
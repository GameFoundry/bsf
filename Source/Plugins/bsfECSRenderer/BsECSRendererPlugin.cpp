//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsECSRendererPrerequisites.h"
#include "BsECSRenderer.h"
#include "Renderer/BsRendererManager.h"

namespace bs
{
	/**	Returns a name of the plugin. */
	extern "C" BS_PLUGIN_EXPORT const char* getPluginName()
	{
		return ECSRendererFactory::SystemName;
	}

	/**	Entry point to the plugin. Called by the engine when the plugin is loaded. */
	extern "C" BS_PLUGIN_EXPORT void* loadPlugin()
	{
		RendererManager::instance()._registerFactory(
			bs_shared_ptr_new<ECSRendererFactory>());

		return nullptr;
	}
}

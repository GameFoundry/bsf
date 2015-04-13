#include "BsSLPrerequisites.h"
#include "BsSLImporter.h"
#include "BsImporter.h"

namespace BansheeEngine
{
	/**
	 * @brief	Returns a name of the plugin.
	 */
	extern "C" BS_SL_EXPORT const String& getPluginName()
	{
		return SystemName;
	}

	/**
	 * @brief	Entry point to the plugin. Called by the engine when the plugin is loaded.
	 */
	extern "C" BS_SL_EXPORT void* loadPlugin()
	{
		SLImporter* importer = bs_new<SLImporter>();
		Importer::instance()._registerAssetImporter(importer);

		return nullptr;
	}
}
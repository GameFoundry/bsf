#include "CmFBXPrerequisites.h"
#include "CmFBXImporter.h"
#include "CmImporter.h"

namespace BansheeEngine
{
	/**
	 * @brief	Returns a name of the plugin.
	 */
	extern "C" BS_FBX_EXPORT const String& getPluginName()
	{
		static String pluginName = "FBXImporter";
		return pluginName;
	}

	/**
	 * @brief	Entry point to the plugin. Called by the engine when the plugin is loaded.
	 */
	extern "C" BS_FBX_EXPORT void* loadPlugin()
	{
		FBXImporter* importer = bs_new<FBXImporter>();
		Importer::instance()._registerAssetImporter(importer);

		return nullptr;
	}
}
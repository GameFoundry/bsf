#include "CmFontPrerequisites.h"
#include "CmImporter.h"
#include "CmFontImporter.h"

namespace BansheeEngine
{
	/**
	 * @brief	Returns a name of the plugin.
	 */
	extern "C" BS_FONT_EXPORT const String& getPluginName()
	{
		static String pluginName = "FontImporter";
		return pluginName;
	}

	/**
	 * @brief	Entry point to the plugin. Called by the engine when the plugin is loaded.
	 */
	extern "C" BS_FONT_EXPORT void* loadPlugin()
	{
		FontImporter* importer = cm_new<FontImporter>();
		Importer::instance()._registerAssetImporter(importer);

		return nullptr;
	}
}
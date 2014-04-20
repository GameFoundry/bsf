#include "CmFontPrerequisites.h"
#include "CmFontImporter.h"

namespace BansheeEngine
{
	extern "C" CM_FONT_EXPORT const String& getPluginName()
	{
		static String pluginName = "FontImporter";
		return pluginName;
	}

	extern "C" CM_FONT_EXPORT void* loadPlugin()
	{
		FontImporter::startUp();

		return nullptr;
	}
}
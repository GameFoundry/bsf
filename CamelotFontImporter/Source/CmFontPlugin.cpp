#include "CmFontPrerequisites.h"
#include "CmFontImporter.h"

namespace CamelotEngine
{
	extern "C" CM_FONT_EXPORT const String& getPluginName()
	{
		static String pluginName = "FontImporter";
		return pluginName;
	}

	extern "C" CM_FONT_EXPORT void loadPlugin()
	{
		FontImporter::startUp();
	}
}
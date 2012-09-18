#include "CmFreeImgPrerequisites.h"

namespace CamelotEngine
{
	extern "C" CM_FREEIMG_EXPORT const String& getPluginName()
	{
		static String pluginName = "FreeImageImporter";
		return pluginName;
	}
}
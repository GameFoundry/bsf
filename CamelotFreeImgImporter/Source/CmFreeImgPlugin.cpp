#include "CmFreeImgPrerequisites.h"
#include "CmFreeImgImporter.h"

namespace CamelotEngine
{
	extern "C" CM_FREEIMG_EXPORT const String& getPluginName()
	{
		static String pluginName = "FreeImageImporter";
		return pluginName;
	}

	extern "C" CM_FREEIMG_EXPORT void* loadPlugin()
	{
		FreeImgImporter::startUp();

		return nullptr;
	}
}
#include "CmFreeImgPrerequisites.h"
#include "CmFreeImgImporter.h"

namespace BansheeEngine
{
	extern "C" BS_FREEIMG_EXPORT const String& getPluginName()
	{
		static String pluginName = "FreeImageImporter";
		return pluginName;
	}

	extern "C" BS_FREEIMG_EXPORT void* loadPlugin()
	{
		FreeImgImporter::startUp();

		return nullptr;
	}
}
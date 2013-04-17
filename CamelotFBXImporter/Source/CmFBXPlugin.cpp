#include "CmFBXPrerequisites.h"
#include "CmFBXImporter.h"

namespace CamelotEngine
{
	extern "C" CM_FBX_EXPORT const String& getPluginName()
	{
		static String pluginName = "FBXImporter";
		return pluginName;
	}

	extern "C" CM_FBX_EXPORT void* loadPlugin()
	{
		FBXImporter::startUp();

		return nullptr;
	}
}
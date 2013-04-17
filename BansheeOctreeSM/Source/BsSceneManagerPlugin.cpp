#include "BsOctreeSMPrerequisites.h"
#include "BsOctreeSceneManager.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	extern "C" BS_SM_EXPORT const String& getPluginName()
	{
		static String pluginName = "BansheeOctreeSM";
		return pluginName;
	}

	extern "C" BS_SM_EXPORT void* loadPlugin()
	{
		return CM_NEW(OctreeSceneManager, GenAlloc) OctreeSceneManager();
	}
}
#include "BsMonoPrerequisites.h"

namespace BansheeEngine
{
	extern "C" BS_MONO_EXPORT const String& getPluginName()
	{
		static String pluginName = "BansheeMono";
		return pluginName;
	}
}
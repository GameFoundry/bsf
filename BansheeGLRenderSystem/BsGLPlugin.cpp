#include "BsGLPrerequisites.h"
#include "BsGLRenderSystemFactory.h"

namespace BansheeEngine
{
	extern "C" BS_RSGL_EXPORT const String& getPluginName()
	{
		return SystemName;
	}
}
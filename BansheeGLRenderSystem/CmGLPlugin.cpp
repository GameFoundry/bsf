#include "CmGLPrerequisites.h"
#include "CmGLRenderSystemFactory.h"

namespace BansheeEngine
{
	extern "C" BS_RSGL_EXPORT const String& getPluginName()
	{
		return SystemName;
	}
}
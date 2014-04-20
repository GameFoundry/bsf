#include "CmGLPrerequisites.h"
#include "CmGLRenderSystemFactory.h"

namespace BansheeEngine
{
	extern "C" CM_RSGL_EXPORT const String& getPluginName()
	{
		return SystemName;
	}
}
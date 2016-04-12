#include "BsGLPrerequisites.h"
#include "BsGLRenderAPIFactory.h"

namespace BansheeEngine
{
	extern "C" BS_RSGL_EXPORT const char* getPluginName()
	{
		return SystemName;
	}
}
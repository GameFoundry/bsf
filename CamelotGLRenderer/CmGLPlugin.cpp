#include "CmGLPrerequisites.h"
#include "CmGLRenderSystemFactory.h"

namespace CamelotEngine
{
	extern "C" CM_RSGL_EXPORT const String& getPluginName()
	{
		return SystemName;
	}
}
#include "CmD3D9Prerequisites.h"
#include "CmD3D9RenderSystemFactory.h"

namespace BansheeEngine
{
	extern "C" CM_D3D9_EXPORT const String& getPluginName()
	{
		return SystemName;
	}
}
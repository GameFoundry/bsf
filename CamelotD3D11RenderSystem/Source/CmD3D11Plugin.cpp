#include "CmD3D11Prerequisites.h"
#include "CmD3D11RenderSystemFactory.h"

namespace CamelotFramework
{
	extern "C" CM_D3D11_EXPORT const String& getPluginName()
	{
		return SystemName;
	}
}
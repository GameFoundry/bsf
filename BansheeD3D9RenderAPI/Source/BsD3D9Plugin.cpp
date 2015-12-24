#include "BsD3D9Prerequisites.h"
#include "BsD3D9RenderAPIFactory.h"

namespace BansheeEngine
{
	extern "C" BS_D3D9_EXPORT const String& getPluginName()
	{
		return SystemName;
	}
}
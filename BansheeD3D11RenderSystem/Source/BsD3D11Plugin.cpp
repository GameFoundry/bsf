#include "BsD3D11Prerequisites.h"
#include "BsD3D11RenderAPIFactory.h"

namespace BansheeEngine
{
	extern "C" BS_D3D11_EXPORT const String& getPluginName()
	{
		return SystemName;
	}
}
#include "BsD3D11RenderAPIFactory.h"
#include "BsRenderAPI.h"

namespace BansheeEngine
{
	void D3D11RenderAPIFactory::create()
	{
		RenderAPICore::startUp<D3D11RenderAPI>();
	}

	D3D11RenderAPIFactory::InitOnStart D3D11RenderAPIFactory::initOnStart;
}
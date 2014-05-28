#include "CmD3D11RenderSystemFactory.h"
#include "CmRenderSystem.h"

namespace BansheeEngine
{
	void D3D11RenderSystemFactory::create()
	{
		RenderSystem::startUp<D3D11RenderSystem>();
	}

	D3D11RenderSystemFactory::InitOnStart D3D11RenderSystemFactory::initOnStart;
}
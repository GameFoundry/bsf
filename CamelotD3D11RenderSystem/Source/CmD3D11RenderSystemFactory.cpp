#include "CmD3D11RenderSystemFactory.h"
#include "CmRenderSystem.h"

namespace CamelotEngine
{
	void D3D11RenderSystemFactory::create()
	{
		RenderSystem::startUp(CM_NEW(D3D11RenderSystem, GenAlloc) D3D11RenderSystem());
	}

	D3D11RenderSystemFactory::InitOnStart D3D11RenderSystemFactory::initOnStart;
}
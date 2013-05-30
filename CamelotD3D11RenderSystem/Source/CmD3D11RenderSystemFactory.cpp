#include "CmD3D11RenderSystemFactory.h"
#include "CmRenderSystem.h"

namespace CamelotFramework
{
	void D3D11RenderSystemFactory::create()
	{
		RenderSystem::startUp(new (cm_alloc<D3D11RenderSystem>()) D3D11RenderSystem());
	}

	D3D11RenderSystemFactory::InitOnStart D3D11RenderSystemFactory::initOnStart;
}
#include "CmD3D9RenderSystemFactory.h"
#include "CmRenderSystem.h"

namespace CamelotFramework
{
	void D3D9RenderSystemFactory::create()
	{
	#ifdef CM_STATIC_LIB
		HINSTANCE hInst = GetModuleHandle( NULL );
	#else
		HINSTANCE hInst = GetModuleHandle( "CamelotD3D9RenderSystem.dll" );
	#endif

		RenderSystem::startUp(CM_NEW(D3D9RenderSystem, GenAlloc) D3D9RenderSystem(hInst));
	}

	D3D9RenderSystemFactory::InitOnStart D3D9RenderSystemFactory::initOnStart;
}
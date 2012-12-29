#include "CmD3D9DepthStencilBuffer.h"
#include "CmD3D9RenderSystem.h"

namespace CamelotEngine
{
	D3D9DepthStencilBuffer::D3D9DepthStencilBuffer(UINT32 bitDepth, UINT32 width, UINT32 height, UINT32 fsaa, const String &fsaaHint)
		:DepthStencilBuffer(bitDepth, width, height, fsaa, fsaaHint), mSurface(nullptr)
	{
		D3D9RenderSystem* rs = static_cast<D3D9RenderSystem*>(RenderSystem::instancePtr());

		IDirect3DDevice9* d3d9Device = D3D9RenderSystem::getActiveD3D9Device();

		D3DFORMAT format = D3DFMT_D24S8;
		D3DMULTISAMPLE_TYPE msType = D3DMULTISAMPLE_NONE;
		DWORD msQuality = 0;
		rs->determineFSAASettings(d3d9Device, fsaa, fsaaHint, format, false, &msType, &msQuality);

		/// If not, create the depthstencil surface
		HRESULT hr = d3d9Device->CreateDepthStencilSurface( 
			static_cast<UINT>(width), 
			static_cast<UINT>(height), 
			format, 
			msType, 
			msQuality, 
			TRUE,  // discard true or false?
			&mSurface, 
			NULL);

		if(FAILED(hr))
		{
			String msg = DXGetErrorDescription(hr);
			CM_EXCEPT(InvalidParametersException, "Error CreateDepthStencilSurface : " + msg);
		}
	}

	D3D9DepthStencilBuffer::~D3D9DepthStencilBuffer()
	{
		SAFE_RELEASE(mSurface);
	}
}
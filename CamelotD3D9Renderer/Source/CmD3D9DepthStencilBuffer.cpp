#include "CmD3D9DepthStencilBuffer.h"
#include "CmD3D9RenderSystem.h"
#include "CmD3D9Mappings.h"

namespace CamelotEngine
{
	D3D9DepthStencilBuffer::D3D9DepthStencilBuffer(DepthStencilFormat format, UINT32 width, UINT32 height, UINT32 fsaa, const String &fsaaHint)
		:DepthStencilBuffer(format, width, height, fsaa, fsaaHint), mSurface(nullptr)
	{
		D3D9RenderSystem* rs = static_cast<D3D9RenderSystem*>(RenderSystem::instancePtr());

		IDirect3DDevice9* d3d9Device = D3D9RenderSystem::getActiveD3D9Device();

		D3DFORMAT d3dFormat = D3D9Mappings::get(format);
		D3DMULTISAMPLE_TYPE msType = D3DMULTISAMPLE_NONE;
		DWORD msQuality = 0;
		rs->determineFSAASettings(d3d9Device, fsaa, fsaaHint, d3dFormat, false, &msType, &msQuality);

		/// If not, create the depthstencil surface
		HRESULT hr = d3d9Device->CreateDepthStencilSurface( 
			static_cast<UINT>(width), 
			static_cast<UINT>(height), 
			d3dFormat, 
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
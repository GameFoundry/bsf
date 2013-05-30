#include "CmD3D11TextureManager.h"
#include "CmD3D11Texture.h"
#include "CmD3D11RenderTexture.h"
#include "CmD3D11Mappings.h"
#include "CmD3D11RenderSystem.h"
#include "CmD3D11MultiRenderTexture.h"

namespace CamelotFramework
{
	D3D11TextureManager::D3D11TextureManager() 
		:TextureManager()
	{ }

	D3D11TextureManager::~D3D11TextureManager()
	{ }

	TexturePtr D3D11TextureManager::createTextureImpl()
	{
		D3D11Texture* tex = new (cm_alloc<D3D11Texture, PoolAlloc>()) D3D11Texture(); 

		return cm_core_ptr<D3D11Texture, PoolAlloc>(tex);
	}

	RenderTexturePtr D3D11TextureManager::createRenderTextureImpl()
	{
		D3D11RenderTexture* tex = new (cm_alloc<D3D11RenderTexture, PoolAlloc>()) D3D11RenderTexture();

		return cm_core_ptr<D3D11RenderTexture, PoolAlloc>(tex);
	}

	MultiRenderTexturePtr D3D11TextureManager::createMultiRenderTextureImpl()
	{
		D3D11MultiRenderTexture* tex = new (cm_alloc<D3D11MultiRenderTexture, PoolAlloc>()) D3D11MultiRenderTexture();

		return cm_core_ptr<D3D11MultiRenderTexture, PoolAlloc>(tex);
	}

	PixelFormat D3D11TextureManager::getNativeFormat(TextureType ttype, PixelFormat format, int usage)
	{
		// Basic filtering
		DXGI_FORMAT d3dPF = D3D11Mappings::_getPF(D3D11Mappings::_getClosestSupportedPF(format));

		return D3D11Mappings::_getPF(d3dPF);
	}
}
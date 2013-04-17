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
		D3D11Texture* tex = CM_NEW(D3D11Texture, PoolAlloc) D3D11Texture(); 

		return TexturePtr(tex, &CoreObject::_deleteDelayed<D3D11Texture, PoolAlloc>);
	}

	RenderTexturePtr D3D11TextureManager::createRenderTextureImpl()
	{
		D3D11RenderTexture* tex = CM_NEW(D3D11RenderTexture, PoolAlloc) D3D11RenderTexture();

		return RenderTexturePtr(tex, &CoreObject::_deleteDelayed<D3D11RenderTexture, PoolAlloc>);
	}

	MultiRenderTexturePtr D3D11TextureManager::createMultiRenderTextureImpl()
	{
		D3D11MultiRenderTexture* tex = CM_NEW(D3D11MultiRenderTexture, PoolAlloc) D3D11MultiRenderTexture();

		return MultiRenderTexturePtr(tex, &CoreObject::_deleteDelayed<D3D11MultiRenderTexture, PoolAlloc>);
	}

	PixelFormat D3D11TextureManager::getNativeFormat(TextureType ttype, PixelFormat format, int usage)
	{
		// Basic filtering
		DXGI_FORMAT d3dPF = D3D11Mappings::_getPF(D3D11Mappings::_getClosestSupportedPF(format));

		return D3D11Mappings::_getPF(d3dPF);
	}
}
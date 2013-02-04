#include "CmD3D11TextureManager.h"
#include "CmD3D11Texture.h"
#include "CmD3D11RenderTexture.h"
#include "CmD3D11Mappings.h"
#include "CmD3D11RenderSystem.h"
#include "CmD3D11MultiRenderTexture.h"

namespace CamelotEngine
{
	D3D11TextureManager::D3D11TextureManager() 
		:TextureManager()
	{ }

	D3D11TextureManager::~D3D11TextureManager()
	{ }

	Texture* D3D11TextureManager::createTextureImpl()
	{
		return new D3D11Texture(); 
	}

	RenderTexture* D3D11TextureManager::createRenderTextureImpl()
	{
		return new D3D11RenderTexture();
	}

	MultiRenderTexture* D3D11TextureManager::createMultiRenderTextureImpl()
	{
		return new D3D11MultiRenderTexture();
	}

	PixelFormat D3D11TextureManager::getNativeFormat(TextureType ttype, PixelFormat format, int usage)
	{
		// Basic filtering
		DXGI_FORMAT d3dPF = D3D11Mappings::_getPF(D3D11Mappings::_getClosestSupportedPF(format));

		return D3D11Mappings::_getPF(d3dPF);
	}
}
#include "CmD3D11TextureManager.h"
#include "CmD3D11Texture.h"
#include "CmD3D11RenderTexture.h"
#include "CmD3D11Mappings.h"
#include "CmD3D11RenderSystem.h"
#include "CmD3D11DepthStencilBuffer.h"
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

	DepthStencilBufferPtr D3D11TextureManager::createDepthStencilBuffer(DepthStencilFormat format, UINT32 width, 
		UINT32 height, UINT32 fsaa, const String& fsaaHint)
	{
		return DepthStencilBufferPtr(new D3D11DepthStencilBuffer(format, width, height, fsaa, fsaaHint));
	}

	MultiRenderTexturePtr D3D11TextureManager::createMultiRenderTexture()
	{
		D3D11MultiRenderTexture* newMRT = new D3D11MultiRenderTexture();
		newMRT->initialize();

		return MultiRenderTexturePtr(newMRT);
	}

	PixelFormat D3D11TextureManager::getNativeFormat(TextureType ttype, PixelFormat format, int usage)
	{
		// Basic filtering
		DXGI_FORMAT d3dPF = D3D11Mappings::_getPF(D3D11Mappings::_getClosestSupportedPF(format));

		return D3D11Mappings::_getPF(d3dPF);
	}

	bool D3D11TextureManager::isHardwareFilteringSupported(TextureType ttype, PixelFormat format, int usage,
		bool preciseFormatOnly)
	{
		if (!preciseFormatOnly)
			format = getNativeFormat(ttype, format, usage);

		D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		return rs->checkTextureFilteringSupported(ttype, format, usage);
	}
}
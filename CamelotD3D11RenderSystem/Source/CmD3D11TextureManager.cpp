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

	TexturePtr D3D11TextureManager::createTextureImpl()
	{
		return TexturePtr(new D3D11Texture()); 
	}

	RenderTexturePtr D3D11TextureManager::createRenderTextureImpl()
	{
		return RenderTexturePtr(new D3D11RenderTexture());
	}

	MultiRenderTexturePtr D3D11TextureManager::createMultiRenderTextureImpl()
	{
		return MultiRenderTexturePtr(new D3D11MultiRenderTexture());
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
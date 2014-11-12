#include "BsD3D11RenderStateManager.h"
#include "BsD3D11SamplerState.h"
#include "BsD3D11DepthStencilState.h"
#include "BsD3D11RasterizerState.h"
#include "BsD3D11BlendState.h"

namespace BansheeEngine
{
	SPtr<SamplerStateCore> D3D11RenderStateCoreManager::createSamplerStateInternal(const SAMPLER_STATE_DESC& desc) const
	{
		return bs_shared_ptr<D3D11SamplerStateCore, GenAlloc>(new (bs_alloc<D3D11SamplerStateCore>()) D3D11SamplerStateCore(desc));
	}

	SPtr<BlendStateCore> D3D11RenderStateCoreManager::createBlendStateInternal(const BLEND_STATE_DESC& desc) const
	{
		return bs_shared_ptr<D3D11BlendStateCore, GenAlloc>(new (bs_alloc<D3D11BlendStateCore>()) D3D11BlendStateCore(desc));
	}

	SPtr<RasterizerStateCore> D3D11RenderStateCoreManager::createRasterizerStateInternal(const RASTERIZER_STATE_DESC& desc) const
	{
		return bs_shared_ptr<D3D11RasterizerStateCore, GenAlloc>(new (bs_alloc<D3D11RasterizerStateCore>()) D3D11RasterizerStateCore(desc));
	}

	SPtr<DepthStencilStateCore> D3D11RenderStateCoreManager::createDepthStencilStateInternal(const DEPTH_STENCIL_STATE_DESC& desc) const
	{
		return bs_shared_ptr<D3D11DepthStencilStateCore, GenAlloc>(new (bs_alloc<D3D11DepthStencilStateCore>()) D3D11DepthStencilStateCore(desc));
	}
}
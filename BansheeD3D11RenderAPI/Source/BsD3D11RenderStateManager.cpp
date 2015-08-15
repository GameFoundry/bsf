#include "BsD3D11RenderStateManager.h"
#include "BsD3D11SamplerState.h"
#include "BsD3D11DepthStencilState.h"
#include "BsD3D11RasterizerState.h"
#include "BsD3D11BlendState.h"

namespace BansheeEngine
{
	SPtr<SamplerStateCore> D3D11RenderStateCoreManager::createSamplerStateInternal(const SAMPLER_STATE_DESC& desc) const
	{
		SPtr<SamplerStateCore> ret = bs_shared_ptr<D3D11SamplerStateCore>(new (bs_alloc<D3D11SamplerStateCore>()) D3D11SamplerStateCore(desc));
		ret->_setThisPtr(ret);

		return ret;
	}

	SPtr<BlendStateCore> D3D11RenderStateCoreManager::createBlendStateInternal(const BLEND_STATE_DESC& desc) const
	{
		SPtr<BlendStateCore> ret = bs_shared_ptr<D3D11BlendStateCore>(new (bs_alloc<D3D11BlendStateCore>()) D3D11BlendStateCore(desc));
		ret->_setThisPtr(ret);

		return ret;
	}

	SPtr<RasterizerStateCore> D3D11RenderStateCoreManager::createRasterizerStateInternal(const RASTERIZER_STATE_DESC& desc) const
	{
		SPtr<RasterizerStateCore> ret = bs_shared_ptr<D3D11RasterizerStateCore>(new (bs_alloc<D3D11RasterizerStateCore>()) D3D11RasterizerStateCore(desc));
		ret->_setThisPtr(ret);

		return ret;
	}

	SPtr<DepthStencilStateCore> D3D11RenderStateCoreManager::createDepthStencilStateInternal(const DEPTH_STENCIL_STATE_DESC& desc) const
	{
		SPtr<DepthStencilStateCore> ret = bs_shared_ptr<D3D11DepthStencilStateCore>(new (bs_alloc<D3D11DepthStencilStateCore>()) D3D11DepthStencilStateCore(desc));
		ret->_setThisPtr(ret);

		return ret;
	}
}
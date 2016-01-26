#include "BsD3D11BlendState.h"
#include "BsD3D11Mappings.h"
#include "BsD3D11RenderSystem.h"
#include "BsD3D11Device.h"
#include "BsRenderStats.h"

namespace BansheeEngine
{
	D3D11BlendState::D3D11BlendState()
		:mBlendState(nullptr)
	{ }

	D3D11BlendState::~D3D11BlendState()
	{
	}

	void D3D11BlendState::initialize_internal()
	{
		D3D11_BLEND_DESC blendStateDesc;
		ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));

		blendStateDesc.AlphaToCoverageEnable = mData.alphaToCoverageEnable;
		blendStateDesc.IndependentBlendEnable = mData.independantBlendEnable;
		
		for(UINT32 i = 0; i < BS_MAX_MULTIPLE_RENDER_TARGETS; i++)
		{
			blendStateDesc.RenderTarget[i].BlendEnable = mData.renderTargetDesc[i].blendEnable;
			blendStateDesc.RenderTarget[i].BlendOp = D3D11Mappings::get(mData.renderTargetDesc[i].blendOp);
			blendStateDesc.RenderTarget[i].BlendOpAlpha = D3D11Mappings::get(mData.renderTargetDesc[i].blendOpAlpha);
			blendStateDesc.RenderTarget[i].DestBlend = D3D11Mappings::get(mData.renderTargetDesc[i].dstBlend);
			blendStateDesc.RenderTarget[i].DestBlendAlpha = D3D11Mappings::get(mData.renderTargetDesc[i].dstBlendAlpha);
			blendStateDesc.RenderTarget[i].RenderTargetWriteMask = 0xf & mData.renderTargetDesc[i].renderTargetWriteMask; // Mask out all but last 4 bits
			blendStateDesc.RenderTarget[i].SrcBlend = D3D11Mappings::get(mData.renderTargetDesc[i].srcBlend);
			blendStateDesc.RenderTarget[i].SrcBlendAlpha = D3D11Mappings::get(mData.renderTargetDesc[i].srcBlendAlpha);
		}

		D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		D3D11Device& device = rs->getPrimaryDevice();
		HRESULT hr = device.getD3D11Device()->CreateBlendState(&blendStateDesc, &mBlendState);

		if(FAILED(hr) || device.hasError())
		{
			String errorDescription = device.getErrorDescription();
			BS_EXCEPT(RenderingAPIException, "Cannot create blend state.\nError Description:" + errorDescription);
		}

		BS_INC_RENDER_STAT_CAT(ResCreated, RenderStatObject_BlendState);

		BlendState::initialize_internal();
	}

	void D3D11BlendState::destroy_internal()
	{
		SAFE_RELEASE(mBlendState);

		BS_INC_RENDER_STAT_CAT(ResDestroyed, RenderStatObject_BlendState);

		BlendState::destroy_internal();
	}
}
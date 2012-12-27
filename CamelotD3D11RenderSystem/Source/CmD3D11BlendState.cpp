#include "CmD3D11BlendState.h"
#include "CmD3D11Mappings.h"
#include "CmD3D11RenderSystem.h"
#include "CmD3D11Device.h"

namespace CamelotEngine
{
	D3D11BlendState::D3D11BlendState()
		:mBlendState(nullptr)
	{ }

	D3D11BlendState::~D3D11BlendState()
	{
		SAFE_RELEASE(mBlendState);
	}

	void D3D11BlendState::initialize(const BLEND_STATE_DESC& desc)
	{
		D3D11_BLEND_DESC blendStateDesc;
		ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));

		blendStateDesc.AlphaToCoverageEnable = desc.alphaToCoverageEnable;
		blendStateDesc.IndependentBlendEnable = desc.independantBlendEnable;
		
		for(UINT32 i = 0; i < CM_MAX_MULTIPLE_RENDER_TARGETS; i++)
		{
			blendStateDesc.RenderTarget[i].BlendEnable = desc.renderTargetDesc[i].blendEnable;
			blendStateDesc.RenderTarget[i].BlendOp = D3D11Mappings::get(desc.renderTargetDesc[i].blendOp);
			blendStateDesc.RenderTarget[i].BlendOpAlpha = D3D11Mappings::get(desc.renderTargetDesc[i].blendOpAlpha);
			blendStateDesc.RenderTarget[i].DestBlend = D3D11Mappings::get(desc.renderTargetDesc[i].dstBlend);
			blendStateDesc.RenderTarget[i].DestBlendAlpha = D3D11Mappings::get(desc.renderTargetDesc[i].dstBlendAlpha);
			blendStateDesc.RenderTarget[i].RenderTargetWriteMask = desc.renderTargetDesc[i].renderTargetWriteMask;
			blendStateDesc.RenderTarget[i].SrcBlend = D3D11Mappings::get(desc.renderTargetDesc[i].srcBlend);
			blendStateDesc.RenderTarget[i].SrcBlendAlpha = D3D11Mappings::get(desc.renderTargetDesc[i].srcBlendAlpha);
		}

		D3D11Device& device = D3D11RenderSystem::getPrimaryDevice();
		device.getD3D11Device()->CreateBlendState(&blendStateDesc, &mBlendState);
	}
}
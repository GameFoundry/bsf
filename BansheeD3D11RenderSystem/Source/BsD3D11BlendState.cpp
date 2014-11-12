#include "BsD3D11BlendState.h"
#include "BsD3D11Mappings.h"
#include "BsD3D11RenderSystem.h"
#include "BsD3D11Device.h"
#include "BsRenderStats.h"

namespace BansheeEngine
{
	D3D11BlendStateCore::D3D11BlendStateCore(const BLEND_STATE_DESC& desc)
		:BlendStateCore(desc), mBlendState(nullptr)
	{ }

	D3D11BlendStateCore::~D3D11BlendStateCore()
	{
	}

	void D3D11BlendStateCore::initialize()
	{
		D3D11_BLEND_DESC blendStateDesc;
		ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));

		blendStateDesc.AlphaToCoverageEnable = mProperties.getAlphaToCoverageEnabled();
		blendStateDesc.IndependentBlendEnable = mProperties.getIndependantBlendEnable();
		
		for(UINT32 i = 0; i < BS_MAX_MULTIPLE_RENDER_TARGETS; i++)
		{
			blendStateDesc.RenderTarget[i].BlendEnable = mProperties.getBlendEnabled(i);
			blendStateDesc.RenderTarget[i].BlendOp = D3D11Mappings::get(mProperties.getBlendOperation(i));
			blendStateDesc.RenderTarget[i].BlendOpAlpha = D3D11Mappings::get(mProperties.getAlphaBlendOperation(i));
			blendStateDesc.RenderTarget[i].DestBlend = D3D11Mappings::get(mProperties.getDstBlend(i));
			blendStateDesc.RenderTarget[i].DestBlendAlpha = D3D11Mappings::get(mProperties.getAlphaDstBlend(i));
			blendStateDesc.RenderTarget[i].RenderTargetWriteMask = 0xf & (mProperties.getRenderTargetWriteMask(i)); // Mask out all but last 4 bits
			blendStateDesc.RenderTarget[i].SrcBlend = D3D11Mappings::get(mProperties.getSrcBlend(i));
			blendStateDesc.RenderTarget[i].SrcBlendAlpha = D3D11Mappings::get(mProperties.getAlphaSrcBlend(i));
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

		BlendStateCore::initialize();
	}

	void D3D11BlendStateCore::destroy()
	{
		SAFE_RELEASE(mBlendState);

		BS_INC_RENDER_STAT_CAT(ResDestroyed, RenderStatObject_BlendState);

		BlendStateCore::destroy();
	}
}
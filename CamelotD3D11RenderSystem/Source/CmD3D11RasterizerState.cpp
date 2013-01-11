#include "CmD3D11RasterizerState.h"
#include "CmD3D11RenderSystem.h"
#include "CmD3D11Device.h"
#include "CmD3D11Mappings.h"

namespace CamelotEngine
{
	D3D11RasterizerState::D3D11RasterizerState()
		:mRasterizerState(nullptr)
	{ }

	D3D11RasterizerState::~D3D11RasterizerState()
	{
		SAFE_RELEASE(mRasterizerState);
	}

	void D3D11RasterizerState::initialize(const RASTERIZER_STATE_DESC& desc)
	{
		D3D11_RASTERIZER_DESC rasterizerStateDesc;
		ZeroMemory(&rasterizerStateDesc, sizeof(D3D11_RASTERIZER_DESC));

		rasterizerStateDesc.AntialiasedLineEnable = desc.antialiasedLineEnable;
		rasterizerStateDesc.CullMode = D3D11Mappings::get(desc.cullMode);
		rasterizerStateDesc.DepthBias = desc.depthBias;
		rasterizerStateDesc.DepthBiasClamp = desc.depthBiasClamp;
		rasterizerStateDesc.DepthClipEnable = desc.depthClipEnable;
		rasterizerStateDesc.FillMode = D3D11Mappings::get(desc.polygonMode);
		rasterizerStateDesc.MultisampleEnable = desc.multisampleEnable;
		rasterizerStateDesc.ScissorEnable = desc.scissorEnable;
		rasterizerStateDesc.SlopeScaledDepthBias = desc.slopeScaledDepthBias;

		D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		D3D11Device& device = rs->getPrimaryDevice();
		HRESULT hr = device.getD3D11Device()->CreateRasterizerState(&rasterizerStateDesc, &mRasterizerState);

		if(FAILED(hr) || device.hasError())
		{
			String errorDescription = device.getErrorDescription();
			CM_EXCEPT(RenderingAPIException, "Cannot create rasterizer state.\nError Description:" + errorDescription);
		}
	}
}
#include "CmD3D11RasterizerState.h"
#include "CmD3D11RenderSystem.h"
#include "CmD3D11Device.h"
#include "CmD3D11Mappings.h"

namespace BansheeEngine
{
	D3D11RasterizerState::D3D11RasterizerState()
		:mRasterizerState(nullptr)
	{ }

	D3D11RasterizerState::~D3D11RasterizerState()
	{
	}

	void D3D11RasterizerState::initialize_internal()
	{
		D3D11_RASTERIZER_DESC rasterizerStateDesc;
		ZeroMemory(&rasterizerStateDesc, sizeof(D3D11_RASTERIZER_DESC));

		rasterizerStateDesc.AntialiasedLineEnable = mData.antialiasedLineEnable;
		rasterizerStateDesc.CullMode = D3D11Mappings::get(mData.cullMode);
		rasterizerStateDesc.DepthBias = mData.depthBias;
		rasterizerStateDesc.DepthBiasClamp = mData.depthBiasClamp;
		rasterizerStateDesc.DepthClipEnable = mData.depthClipEnable;
		rasterizerStateDesc.FillMode = D3D11Mappings::get(mData.polygonMode);
		rasterizerStateDesc.MultisampleEnable = mData.multisampleEnable;
		rasterizerStateDesc.ScissorEnable = mData.scissorEnable;
		rasterizerStateDesc.SlopeScaledDepthBias = mData.slopeScaledDepthBias;
		rasterizerStateDesc.FrontCounterClockwise = false;

		D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		D3D11Device& device = rs->getPrimaryDevice();
		HRESULT hr = device.getD3D11Device()->CreateRasterizerState(&rasterizerStateDesc, &mRasterizerState);

		if(FAILED(hr) || device.hasError())
		{
			String errorDescription = device.getErrorDescription();
			BS_EXCEPT(RenderingAPIException, "Cannot create rasterizer state.\nError Description:" + errorDescription);
		}

		RasterizerState::initialize_internal();
	}

	void D3D11RasterizerState::destroy_internal()
	{
		SAFE_RELEASE(mRasterizerState);

		RasterizerState::destroy_internal();
	}
}
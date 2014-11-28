#include "BsD3D11RasterizerState.h"
#include "BsD3D11RenderAPI.h"
#include "BsD3D11Device.h"
#include "BsD3D11Mappings.h"
#include "BsRenderStats.h"

namespace BansheeEngine
{
	D3D11RasterizerStateCore::D3D11RasterizerStateCore(const RASTERIZER_STATE_DESC& desc)
		:RasterizerStateCore(desc), mRasterizerState(nullptr)
	{ }

	D3D11RasterizerStateCore::~D3D11RasterizerStateCore()
	{
		SAFE_RELEASE(mRasterizerState);

		BS_INC_RENDER_STAT_CAT(ResDestroyed, RenderStatObject_RasterizerState);
	}

	void D3D11RasterizerStateCore::initialize()
	{
		D3D11_RASTERIZER_DESC rasterizerStateDesc;
		ZeroMemory(&rasterizerStateDesc, sizeof(D3D11_RASTERIZER_DESC));

		rasterizerStateDesc.AntialiasedLineEnable = mProperties.getAntialiasedLineEnable();
		rasterizerStateDesc.CullMode = D3D11Mappings::get(mProperties.getCullMode());
		rasterizerStateDesc.DepthBias = mProperties.getDepthBias();
		rasterizerStateDesc.DepthBiasClamp = mProperties.getDepthBiasClamp();
		rasterizerStateDesc.DepthClipEnable = mProperties.getDepthClipEnable();
		rasterizerStateDesc.FillMode = D3D11Mappings::get(mProperties.getPolygonMode());
		rasterizerStateDesc.MultisampleEnable = mProperties.getMultisampleEnable();
		rasterizerStateDesc.ScissorEnable = mProperties.getScissorEnable();
		rasterizerStateDesc.SlopeScaledDepthBias = mProperties.getSlopeScaledDepthBias();
		rasterizerStateDesc.FrontCounterClockwise = false;

		D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPICore::instancePtr());
		D3D11Device& device = rs->getPrimaryDevice();
		HRESULT hr = device.getD3D11Device()->CreateRasterizerState(&rasterizerStateDesc, &mRasterizerState);

		if(FAILED(hr) || device.hasError())
		{
			String errorDescription = device.getErrorDescription();
			BS_EXCEPT(RenderingAPIException, "Cannot create rasterizer state.\nError Description:" + errorDescription);
		}

		BS_INC_RENDER_STAT_CAT(ResCreated, RenderStatObject_RasterizerState);
		RasterizerStateCore::initialize();
	}
}
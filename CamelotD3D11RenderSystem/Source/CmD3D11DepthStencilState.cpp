#include "CmD3D11DepthStencilState.h"
#include "CmD3D11Device.h"
#include "CmD3D11RenderSystem.h"
#include "CmD3D11Mappings.h"

namespace CamelotEngine
{
	D3D11DepthStencilState::D3D11DepthStencilState()
		:mDepthStencilState(nullptr)
	{ }

	D3D11DepthStencilState::~D3D11DepthStencilState()
	{
		SAFE_RELEASE(mDepthStencilState);
	}

	void D3D11DepthStencilState::initialize(const DEPTH_STENCIL_STATE_DESC& desc)
	{
		D3D11_DEPTH_STENCIL_DESC depthStencilState;
		ZeroMemory(&depthStencilState, sizeof(D3D11_DEPTH_STENCIL_DESC));

		depthStencilState.BackFace.StencilPassOp = D3D11Mappings::get(desc.backStencilPassOp);
		depthStencilState.BackFace.StencilFailOp = D3D11Mappings::get(desc.backStencilFailOp);
		depthStencilState.BackFace.StencilDepthFailOp = D3D11Mappings::get(desc.backStencilZFailOp);
		depthStencilState.BackFace.StencilFunc = D3D11Mappings::get(desc.backStencilComparisonFunc);
		depthStencilState.FrontFace.StencilPassOp = D3D11Mappings::get(desc.frontStencilPassOp);
		depthStencilState.FrontFace.StencilFailOp = D3D11Mappings::get(desc.frontStencilFailOp);
		depthStencilState.FrontFace.StencilDepthFailOp = D3D11Mappings::get(desc.frontStencilZFailOp);
		depthStencilState.FrontFace.StencilFunc = D3D11Mappings::get(desc.frontStencilComparisonFunc);
		depthStencilState.DepthEnable = desc.depthReadEnable;
		depthStencilState.DepthWriteMask = desc.depthWriteEnable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilState.DepthFunc = D3D11Mappings::get(desc.depthComparisonFunc);
		depthStencilState.StencilEnable = desc.stencilEnable;
		depthStencilState.StencilReadMask = desc.stencilReadMask;
		depthStencilState.StencilWriteMask = desc.stencilWriteMask;

		D3D11Device& device = D3D11RenderSystem::getPrimaryDevice();
		device.getD3D11Device()->CreateDepthStencilState(&depthStencilState, &mDepthStencilState);
	}
}
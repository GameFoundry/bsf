//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsD3D11DepthStencilState.h"
#include "BsD3D11Device.h"
#include "BsD3D11RenderSystem.h"
#include "BsD3D11Mappings.h"
#include "BsRenderStats.h"

namespace BansheeEngine
{
	D3D11DepthStencilState::D3D11DepthStencilState()
		:mDepthStencilState(nullptr)
	{ }

	D3D11DepthStencilState::~D3D11DepthStencilState()
	{

	}

	void D3D11DepthStencilState::initialize_internal()
	{
		D3D11_DEPTH_STENCIL_DESC depthStencilState;
		ZeroMemory(&depthStencilState, sizeof(D3D11_DEPTH_STENCIL_DESC));

		depthStencilState.BackFace.StencilPassOp = D3D11Mappings::get(mData.backStencilPassOp);
		depthStencilState.BackFace.StencilFailOp = D3D11Mappings::get(mData.backStencilFailOp);
		depthStencilState.BackFace.StencilDepthFailOp = D3D11Mappings::get(mData.backStencilZFailOp);
		depthStencilState.BackFace.StencilFunc = D3D11Mappings::get(mData.backStencilComparisonFunc);
		depthStencilState.FrontFace.StencilPassOp = D3D11Mappings::get(mData.frontStencilPassOp);
		depthStencilState.FrontFace.StencilFailOp = D3D11Mappings::get(mData.frontStencilFailOp);
		depthStencilState.FrontFace.StencilDepthFailOp = D3D11Mappings::get(mData.frontStencilZFailOp);
		depthStencilState.FrontFace.StencilFunc = D3D11Mappings::get(mData.frontStencilComparisonFunc);
		depthStencilState.DepthEnable = mData.depthReadEnable;
		depthStencilState.DepthWriteMask = mData.depthWriteEnable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilState.DepthFunc = D3D11Mappings::get(mData.depthComparisonFunc);
		depthStencilState.StencilEnable = mData.stencilEnable;
		depthStencilState.StencilReadMask = mData.stencilReadMask;
		depthStencilState.StencilWriteMask = mData.stencilWriteMask;

		D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		D3D11Device& device = rs->getPrimaryDevice();
		HRESULT hr = device.getD3D11Device()->CreateDepthStencilState(&depthStencilState, &mDepthStencilState);

		if(FAILED(hr) || device.hasError())
		{
			String errorDescription = device.getErrorDescription();
			BS_EXCEPT(RenderingAPIException, "Cannot create depth stencil state.\nError Description:" + errorDescription);
		}

		BS_INC_RENDER_STAT_CAT(ResCreated, RenderStatObject_DepthStencilState);

		DepthStencilState::initialize_internal();
	}

	void D3D11DepthStencilState::destroy_internal()
	{
		SAFE_RELEASE(mDepthStencilState);

		BS_INC_RENDER_STAT_CAT(ResDestroyed, RenderStatObject_DepthStencilState);

		DepthStencilState::destroy_internal();
	}
}
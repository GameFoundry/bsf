#include "CmD3D11SamplerState.h"
#include "CmD3D11RenderSystem.h"
#include "CmD3D11Device.h"
#include "CmD3D11Mappings.h"

namespace CamelotEngine
{
	D3D11SamplerState::D3D11SamplerState()
		:mSamplerState(nullptr)
	{ }

	D3D11SamplerState::~D3D11SamplerState()
	{
	}

	void D3D11SamplerState::initialize_internal()
	{
		D3D11_SAMPLER_DESC samplerState;
		ZeroMemory(&samplerState, sizeof(D3D11_SAMPLER_DESC));

		samplerState.AddressU = D3D11Mappings::get(mData.addressMode.u);
		samplerState.AddressV = D3D11Mappings::get(mData.addressMode.v);
		samplerState.AddressW = D3D11Mappings::get(mData.addressMode.w);
		samplerState.BorderColor[0] = mData.borderColor[0];
		samplerState.BorderColor[1] = mData.borderColor[1];
		samplerState.BorderColor[2] = mData.borderColor[2];
		samplerState.BorderColor[3] = mData.borderColor[3];
		samplerState.ComparisonFunc = D3D11Mappings::get(mData.comparisonFunc);
		samplerState.MaxAnisotropy = mData.maxAniso;
		samplerState.MaxLOD = mData.mipMax;
		samplerState.MinLOD = mData.mipMin;
		samplerState.MipLODBias =mData.mipmapBias;

		bool isComparison = ((mData.minFilter & FO_USE_COMPARISON) & (mData.magFilter & FO_USE_COMPARISON) & (mData.mipFilter & FO_USE_COMPARISON)) != 0;

		FilterOptions minFilter = (FilterOptions)(mData.minFilter & ~FO_USE_COMPARISON);
		FilterOptions magFilter = (FilterOptions)(mData.magFilter & ~FO_USE_COMPARISON);
		FilterOptions mipFilter = (FilterOptions)(mData.mipFilter & ~FO_USE_COMPARISON);

		if(minFilter == FO_ANISOTROPIC && mData.magFilter == FO_ANISOTROPIC && mData.mipFilter == FO_ANISOTROPIC)
		{
			samplerState.Filter = D3D11_FILTER_ANISOTROPIC;
		}
		else
		{
			if(minFilter == FO_POINT || minFilter == FO_NONE)
			{
				if(magFilter == FO_POINT || magFilter == FO_NONE)
				{
					if(mipFilter == FO_POINT || mipFilter == FO_NONE)
						samplerState.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
					else if(mipFilter == FO_LINEAR)
						samplerState.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
				}
				else if(magFilter == FO_LINEAR)
				{
					if(mipFilter == FO_POINT || mipFilter == FO_NONE)
						samplerState.Filter = D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
					else if(mipFilter == FO_LINEAR)
						samplerState.Filter = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
				}
			}
			else if(minFilter == FO_LINEAR)
			{
				if(magFilter == FO_POINT || magFilter == FO_NONE)
				{
					if(mipFilter == FO_POINT || mipFilter == FO_NONE)
						samplerState.Filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
					else if(mipFilter == FO_LINEAR)
						samplerState.Filter = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
				}
				else if(magFilter == FO_LINEAR)
				{
					if(mipFilter == FO_POINT || mipFilter == FO_NONE)
						samplerState.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
					else if(mipFilter == FO_LINEAR)
						samplerState.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				}
			}
		}

		if(isComparison)
		{
			// Adds COMPARISON flag to the filter
			// See: http://msdn.microsoft.com/en-us/library/windows/desktop/ff476132(v=vs.85).aspx
			samplerState.Filter = (D3D11_FILTER)(0x80 | samplerState.Filter);
		}

		D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		D3D11Device& device = rs->getPrimaryDevice();
		HRESULT hr = device.getD3D11Device()->CreateSamplerState(&samplerState, &mSamplerState);

		if(FAILED(hr) || device.hasError())
		{
			String errorDescription = device.getErrorDescription();
			CM_EXCEPT(RenderingAPIException, "Cannot create sampler state.\nError Description:" + errorDescription);
		}

		SamplerState::initialize_internal();
	}

	void D3D11SamplerState::destroy_internal()
	{
		SAFE_RELEASE(mSamplerState);

		SamplerState::destroy_internal();
	}
}
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
		SAFE_RELEASE(mSamplerState);
	}

	void D3D11SamplerState::initialize(const SAMPLER_STATE_DESC& desc)
	{
		D3D11_SAMPLER_DESC samplerState;
		ZeroMemory(&samplerState, sizeof(D3D11_SAMPLER_DESC));

		samplerState.AddressU = D3D11Mappings::get(desc.addressMode.u);
		samplerState.AddressV = D3D11Mappings::get(desc.addressMode.v);
		samplerState.AddressW = D3D11Mappings::get(desc.addressMode.w);
		samplerState.BorderColor[0] = desc.borderColor[0];
		samplerState.BorderColor[1] = desc.borderColor[1];
		samplerState.BorderColor[2] = desc.borderColor[2];
		samplerState.BorderColor[3] = desc.borderColor[3];
		samplerState.ComparisonFunc = D3D11Mappings::get(desc.comparisonFunc);
		samplerState.MaxAnisotropy = desc.maxAniso;
		samplerState.MaxLOD = desc.mipMax;
		samplerState.MinLOD = desc.mipMin;
		samplerState.MipLODBias =desc.mipmapBias;

		bool isComparison = ((desc.minFilter & FO_USE_COMPARISON) & (desc.magFilter & FO_USE_COMPARISON) & (desc.mipFilter & FO_USE_COMPARISON)) != 0;

		FilterOptions minFilter = (FilterOptions)(desc.minFilter & ~FO_USE_COMPARISON);
		FilterOptions magFilter = (FilterOptions)(desc.magFilter & ~FO_USE_COMPARISON);
		FilterOptions mipFilter = (FilterOptions)(desc.mipFilter & ~FO_USE_COMPARISON);

		if(minFilter == FO_ANISOTROPIC && desc.magFilter == FO_ANISOTROPIC && desc.mipFilter == FO_ANISOTROPIC)
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

		D3D11Device& device = D3D11RenderSystem::getPrimaryDevice();
		device.getD3D11Device()->CreateSamplerState(&samplerState, &mSamplerState);
	}
}
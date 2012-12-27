#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmSamplerState.h"

namespace CamelotEngine
{
	class CM_D3D11_EXPORT D3D11SamplerState : public SamplerState
	{
	public:
		~D3D11SamplerState();
		ID3D11SamplerState* getInternal() const { return mSamplerState; }

	protected:
		friend class D3D11RenderStateManager;

		D3D11SamplerState();

		void initialize(const SAMPLER_STATE_DESC& desc);

		ID3D11SamplerState* mSamplerState;
	};
}
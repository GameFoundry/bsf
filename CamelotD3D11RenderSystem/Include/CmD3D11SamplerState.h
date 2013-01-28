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

		/**
		 * @copydoc SamplerState::initialize_internal().
		 */
		void initialize_internal();

		/**
		 * @copydoc SamplerState::destroy_internal().
		 */
		void destroy_internal();

		ID3D11SamplerState* mSamplerState;
	};
}
#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmBlendState.h"

namespace CamelotEngine
{
	class CM_D3D11_EXPORT D3D11BlendState : public BlendState
	{
	public:
		~D3D11BlendState();

		ID3D11BlendState* getInternal() const { return mBlendState; }

	protected:
		friend class D3D11RenderStateManager;

		D3D11BlendState();

		void initialize_internal();

		ID3D11BlendState* mBlendState;
	};
}
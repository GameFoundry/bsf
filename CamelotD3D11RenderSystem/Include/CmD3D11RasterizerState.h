#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmRasterizerState.h"

namespace CamelotEngine
{
	class CM_D3D11_EXPORT D3D11RasterizerState : public RasterizerState
	{
	public:
		~D3D11RasterizerState();
		ID3D11RasterizerState* getInternal() const { return mRasterizerState; }

	protected:
		friend class D3D11RenderStateManager;

		D3D11RasterizerState();

		void initialize(const RASTERIZER_STATE_DESC& desc);

		ID3D11RasterizerState* mRasterizerState;
	};
}
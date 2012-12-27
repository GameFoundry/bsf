#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmDepthStencilState.h"

namespace CamelotEngine
{
	class CM_D3D11_EXPORT D3D11DepthStencilState : public DepthStencilState
	{
	public:
		~D3D11DepthStencilState();

		ID3D11DepthStencilState* getInternal() const { return mDepthStencilState; }

	protected:
		friend class D3D11RenderStateManager;

		D3D11DepthStencilState();

		void initialize(const DEPTH_STENCIL_STATE_DESC& desc);

		ID3D11DepthStencilState* mDepthStencilState;
	};
}
#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmDepthStencilState.h"

namespace BansheeEngine
{
	class BS_D3D11_EXPORT D3D11DepthStencilState : public DepthStencilState
	{
	public:
		~D3D11DepthStencilState();

		ID3D11DepthStencilState* getInternal() const { return mDepthStencilState; }

	protected:
		friend class D3D11RenderStateManager;

		D3D11DepthStencilState();

		/**
		 * @copydoc DepthStencilState::initialize_internal().
		 */
		void initialize_internal();

		/**
		 * @copydoc DepthStencilState::destroy_internal().
		 */
		void destroy_internal();

		ID3D11DepthStencilState* mDepthStencilState;
	};
}
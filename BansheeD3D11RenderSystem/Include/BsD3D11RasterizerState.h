#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsRasterizerState.h"

namespace BansheeEngine
{
	class BS_D3D11_EXPORT D3D11RasterizerState : public RasterizerState
	{
	public:
		~D3D11RasterizerState();
		ID3D11RasterizerState* getInternal() const { return mRasterizerState; }

	protected:
		friend class D3D11RenderStateManager;

		D3D11RasterizerState();

		/**
		 * @copydoc RasterizerState::initialize_internal().
		 */
		void initialize_internal();

		/**
		 * @copydoc RasterizerState::destroy_internal().
		 */
		void destroy_internal();

		ID3D11RasterizerState* mRasterizerState;
	};
}
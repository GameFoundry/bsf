#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsBlendState.h"

namespace BansheeEngine
{
	class BS_D3D11_EXPORT D3D11BlendState : public BlendState
	{
	public:
		~D3D11BlendState();

		ID3D11BlendState* getInternal() const { return mBlendState; }

	protected:
		friend class D3D11RenderStateManager;

		D3D11BlendState();

		/**
		 * @copydoc BlendState::initialize_internal().
		 */
		void initialize_internal();

		/**
		 * @copydoc BlendState::destroy_internal().
		 */
		void destroy_internal();

		ID3D11BlendState* mBlendState;
	};
}
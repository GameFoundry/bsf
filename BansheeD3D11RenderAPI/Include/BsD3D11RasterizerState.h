#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsRasterizerState.h"

namespace BansheeEngine
{
	/**
	 * @brief	DirectX 11 implementation of a rasterizer state. Wraps a DX11
	 *			rasterizer state object.
	 */
	class BS_D3D11_EXPORT D3D11RasterizerStateCore : public RasterizerStateCore
	{
	public:
		~D3D11RasterizerStateCore();
		ID3D11RasterizerState* getInternal() const { return mRasterizerState; }

	protected:
		friend class D3D11RenderStateCoreManager;

		D3D11RasterizerStateCore(const RASTERIZER_STATE_DESC& desc, UINT32 id);

		/**
		 * @copydoc RasterizerStateCore::createInternal
		 */
		void createInternal() override;

		ID3D11RasterizerState* mRasterizerState;
	};
}
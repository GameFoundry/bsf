//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsRasterizerState.h"

namespace BansheeEngine
{
	/**
	 * @brief	DirectX 11 implementation of a rasterizer state. Wraps a DX11
	 *			rasterizer state object.
	 */
	class BS_D3D11_EXPORT D3D11RasterizerState : public RasterizerState
	{
	public:
		~D3D11RasterizerState();
		ID3D11RasterizerState* getInternal() const { return mRasterizerState; }

	protected:
		friend class D3D11RenderStateManager;

		D3D11RasterizerState();

		/**
		 * @copydoc RasterizerState::initialize_internal
		 */
		void initialize_internal();

		/**
		 * @copydoc RasterizerState::destroy_internal
		 */
		void destroy_internal();

		ID3D11RasterizerState* mRasterizerState;
	};
}
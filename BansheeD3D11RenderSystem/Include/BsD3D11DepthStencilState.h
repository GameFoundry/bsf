//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsDepthStencilState.h"

namespace BansheeEngine
{
	/**
	 * @brief	DirectX 11 implementation of a depth stencil state. 
	 *			Corresponds directly with a DX11 depth stencil state object.
	 */
	class BS_D3D11_EXPORT D3D11DepthStencilState : public DepthStencilState
	{
	public:
		~D3D11DepthStencilState();

		/**
		 * @brief	Returns the internal DX11 depth stencil state object.
		 */
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
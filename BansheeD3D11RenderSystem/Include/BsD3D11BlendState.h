//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsBlendState.h"

namespace BansheeEngine
{
	/**
	 * @brief	DirectX 11 implementation of a blend state. 
	 *			Corresponds directly with a DX11 blend state object.
	 */
	class BS_D3D11_EXPORT D3D11BlendState : public BlendState
	{
	public:
		~D3D11BlendState();

		/**
		 * @brief	Returns the internal DX11 blend state object.
		 */
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
//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsRenderStateManager.h"

namespace BansheeEngine
{
	/**
	 * @brief	Handles creation of DirectX 11 pipeline states.
	 */
	class BS_D3D11_EXPORT D3D11RenderStateCoreManager : public RenderStateCoreManager
	{
	protected:
		/**
		 * @copydoc	RenderStateCoreManager::createSamplerStateInternal
		 */
		virtual SPtr<SamplerStateCore> createSamplerStateInternal(const SAMPLER_STATE_DESC& desc) const override;

		/**
		 * @copydoc	RenderStateCoreManager::createBlendStateInternal
		 */
		virtual SPtr<BlendStateCore> createBlendStateInternal(const BLEND_STATE_DESC& desc, UINT32 id) const override;

		/**
		 * @copydoc	RenderStateCoreManager::createRasterizerStateInternal
		 */
		virtual SPtr<RasterizerStateCore> createRasterizerStateInternal(const RASTERIZER_STATE_DESC& desc, UINT32 id) const override;

		/**
		 * @copydoc	RenderStateCoreManager::createDepthStencilStateInternal
		 */
		virtual SPtr<DepthStencilStateCore> createDepthStencilStateInternal(const DEPTH_STENCIL_STATE_DESC& desc, UINT32 id) const override;
	};
}
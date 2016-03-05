//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsMultiRenderTexture.h"

namespace BansheeEngine
{
	/** @addtogroup D3D11
	 *  @{
	 */

	class D3D11MultiRenderTexture;

	/**
	 * DirectX 11 implementation of a render texture with multiple color surfaces.
	 *
	 * @note	Core thread only.
	 */
	class BS_D3D11_EXPORT D3D11MultiRenderTextureCore : public MultiRenderTextureCore
	{
	public:
		D3D11MultiRenderTextureCore(const MULTI_RENDER_TEXTURE_CORE_DESC& desc);
		virtual ~D3D11MultiRenderTextureCore();
		
		/** @copydoc MultiRenderTextureCore::getCustomAttribute */
		void getCustomAttribute(const String& name, void* pData) const override;

	protected:
		friend class D3D11MultiRenderTexture;

		/** @copydoc MultiRenderTextureCore::getProperties */
		const RenderTargetProperties& getPropertiesInternal() const override { return mProperties; }

		MultiRenderTextureProperties mProperties;
	};

	/**
	 * DirectX 11 implementation of a render texture with multiple color surfaces.
	 *
	 * @note	Sim thread only.
	 */
	class BS_D3D11_EXPORT D3D11MultiRenderTexture : public MultiRenderTexture
	{
	public:
		virtual ~D3D11MultiRenderTexture() { }

	protected:
		friend class D3D11TextureManager;

		D3D11MultiRenderTexture(const MULTI_RENDER_TEXTURE_DESC& desc);

		/** @copydoc MultiRenderTexture::getProperties */
		const RenderTargetProperties& getPropertiesInternal() const override { return mProperties; }

		MultiRenderTextureProperties mProperties;
	};

	/** @} */
}
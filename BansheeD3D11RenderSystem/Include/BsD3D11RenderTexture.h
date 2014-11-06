#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsTexture.h"
#include "BsRenderTexture.h"

namespace BansheeEngine
{
	class D3D11RenderTexture;

	/**
	 * @brief	DirectX 11 implementation of a render texture.
	 *
	 * @note	Core thread only.
	 */
	class D3D11RenderTextureCore : public RenderTextureCore
	{
	public:
		D3D11RenderTextureCore(D3D11RenderTexture* parent, RenderTextureProperties* properties, const RENDER_SURFACE_DESC& colorSurfaceDesc,
			const RENDER_SURFACE_DESC& depthStencilSurfaceDesc);

		virtual ~D3D11RenderTextureCore() { }

		/**
		 * @copydoc	RenderTextureCore::getCustomAttribute
		 */
		void getCustomAttribute(const String& name, void* pData) const;

	protected:
		friend class D3D11RenderTexture;
	};

	/**
	 * @brief	DirectX 11 implementation of a render texture.
	 *
	 * @note	Sim thread only.
	 */
	class D3D11RenderTexture : public RenderTexture
	{
	public:
		virtual ~D3D11RenderTexture() { }

	protected:
		friend class D3D11TextureManager;

		D3D11RenderTexture() { }

		/**
		 * @copydoc	RenderTexture::createProperties
		 */
		virtual RenderTargetProperties* createProperties() const;

		/**
		 * @copydoc	RenderTexture::createCore
		 */
		virtual CoreObjectCore* createCore() const;
	};
}
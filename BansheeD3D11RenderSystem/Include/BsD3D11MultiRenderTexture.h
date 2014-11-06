#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsMultiRenderTexture.h"

namespace BansheeEngine
{
	class D3D11MultiRenderTexture;

	/**
	 * @brief	DirectX 11 implementation of a render texture with multiple color surfaces.
	 *
	 * @note	Core thread only.
	 */
	class BS_D3D11_EXPORT D3D11MultiRenderTextureCore : public MultiRenderTextureCore
	{
	public:
		D3D11MultiRenderTextureCore(D3D11MultiRenderTexture* parent, MultiRenderTextureProperties* properties, const MULTI_RENDER_TEXTURE_DESC& desc);
		virtual ~D3D11MultiRenderTextureCore();
		
		/**
		 * @copydoc	MultiRenderTextureCore::getCustomAttribute
		 */
		void getCustomAttribute(const String& name, void* pData) const;
	protected:
		friend class D3D11MultiRenderTexture;
	};

	/**
	 * @brief	DirectX 11 implementation of a render texture with multiple color surfaces.
	 *
	 * @note	Sim thread only.
	 */
	class BS_D3D11_EXPORT D3D11MultiRenderTexture : public MultiRenderTexture
	{
	public:
		virtual ~D3D11MultiRenderTexture() { }

	protected:
		friend class D3D11TextureManager;

		D3D11MultiRenderTexture() { }

		/**
		 * @copydoc	MultiRenderTexture::createProperties
		 */
		virtual RenderTargetProperties* createProperties() const;

		/**
		 * @copydoc	RenderTexture::createCore
		 */
		virtual CoreObjectCore* createCore() const;
	};
}
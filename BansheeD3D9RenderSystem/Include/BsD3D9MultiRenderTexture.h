#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsMultiRenderTexture.h"

namespace BansheeEngine
{
	class D3D9MultiRenderTexture;

	/**
	 * @brief	DirectX 9 implementation of a render texture with multiple color surfaces.
	 *
	 * @note	Core thread only.
	 */
	class BS_D3D9_EXPORT D3D9MultiRenderTextureCore : public MultiRenderTextureCore
	{
	public:
		D3D9MultiRenderTextureCore(D3D9MultiRenderTexture* parent, MultiRenderTextureProperties* properties, const MULTI_RENDER_TEXTURE_DESC& desc);
		virtual ~D3D9MultiRenderTextureCore();
		
		/**
		 * @copydoc	MultiRenderTextureCore::getCustomAttribute
		 */
		void getCustomAttribute(const String& name, void* pData) const;

	protected:
		friend class D3D9MultiRenderTexture;

		/**
		 * @copydoc	CoreObjectCore::initialize
		 */
		virtual void initialize();

		Vector<IDirect3DSurface9*> mDX9ColorSurfaces;
		IDirect3DSurface9* mDX9DepthStencilSurface;
	};

	/**
	 * @brief	DirectX 9 implementation of a render texture with multiple color surfaces.
	 *
	 * @note	Sim thread only.
	 */
	class BS_D3D9_EXPORT D3D9MultiRenderTexture : public MultiRenderTexture
	{
	public:
		virtual ~D3D9MultiRenderTexture() { }

	protected:
		friend class D3D9TextureManager;

		D3D9MultiRenderTexture() { }

		/**
		 * @copydoc	MultiRenderTexture::createProperties
		 */
		virtual RenderTargetProperties* createProperties() const;

		/**
		 * @copydoc	RenderTexture::createCore
		 */
		virtual SPtr<CoreObjectCore> createCore() const;
	};
}
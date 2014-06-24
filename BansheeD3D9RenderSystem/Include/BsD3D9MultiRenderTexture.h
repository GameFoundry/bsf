#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsMultiRenderTexture.h"

namespace BansheeEngine
{
	/**
	 * @brief	DirectX 9 implementation of a render texture with multiple color surfaces.
	 */
	class BS_D3D9_EXPORT D3D9MultiRenderTexture : public MultiRenderTexture
	{
	public:
		virtual ~D3D9MultiRenderTexture();

		/**
		 * @copydoc	MultiRenderTexture::requiresTextureFlipping
		 */
		bool requiresTextureFlipping() const { return false; }

		/**
		 * @copydoc	MultiRenderTexture::getCustomAttribute
		 */
		void getCustomAttribute(const String& name, void* pData) const;

	protected:
		friend class D3D9TextureManager;

		D3D9MultiRenderTexture();

		/**
		 * @copydoc MultiRenderTexture::initialize_internal().
		 */
		void initialize_internal();

		Vector<IDirect3DSurface9*> mDX9ColorSurfaces;
		IDirect3DSurface9* mDX9DepthStencilSurface;
	};
}
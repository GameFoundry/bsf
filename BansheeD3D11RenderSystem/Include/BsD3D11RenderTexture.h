#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsTexture.h"
#include "BsRenderTexture.h"

namespace BansheeEngine
{
	/**
	 * @brief	DirectX 11 implementation of a render texture.
	 */
	class D3D11RenderTexture : public RenderTexture
	{
	public:
		virtual ~D3D11RenderTexture();

		/**
		 * @copydoc	RenderTexture::requiresTextureFlipping
		 */
		bool requiresTextureFlipping() const { return false; }

		/**
		 * @copydoc	RenderTexture::getCustomAttribute
		 */
		void getCustomAttribute(const String& name, void* pData) const;

	protected:
		friend class D3D11TextureManager;

		D3D11RenderTexture();
	};
}
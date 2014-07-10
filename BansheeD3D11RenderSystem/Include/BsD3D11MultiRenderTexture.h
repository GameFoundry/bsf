//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsMultiRenderTexture.h"

namespace BansheeEngine
{
	/**
	 * @brief	DirectX 11 implementation of a render texture with multiple color surfaces.
	 */
	class BS_D3D11_EXPORT D3D11MultiRenderTexture : public MultiRenderTexture
	{
	public:
		virtual ~D3D11MultiRenderTexture();

		/**
		 * @copydoc	MultiRenderTexture::requiresTextureFlipping
		 */
		bool requiresTextureFlipping() const { return false; }

		/**
		 * @copydoc	MultiRenderTexture::getCustomAttribute
		 */
		void getCustomAttribute(const String& name, void* pData) const;
	protected:
		friend class D3D11TextureManager;

		D3D11MultiRenderTexture();
	};
}
#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsTexture.h"
#include "BsRenderTexture.h"

namespace BansheeEngine
{
	class D3D11RenderTexture : public RenderTexture
	{
	public:
		virtual ~D3D11RenderTexture();

		bool requiresTextureFlipping() const { return false; }
		void getCustomAttribute(const String& name, void* pData) const;

	protected:
		friend class D3D11TextureManager;

		D3D11RenderTexture();
	};
}
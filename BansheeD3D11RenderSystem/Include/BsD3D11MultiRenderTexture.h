#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsMultiRenderTexture.h"

namespace BansheeEngine
{
	class BS_D3D11_EXPORT D3D11MultiRenderTexture : public MultiRenderTexture
	{
	public:
		virtual ~D3D11MultiRenderTexture();

		bool requiresTextureFlipping() const { return false; }
		void getCustomAttribute(const String& name, void* pData) const;
	protected:
		friend class D3D11TextureManager;

		D3D11MultiRenderTexture();
	};
}
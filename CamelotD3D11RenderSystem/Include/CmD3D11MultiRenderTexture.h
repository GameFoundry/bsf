#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmMultiRenderTexture.h"

namespace CamelotEngine
{
	class CM_D3D11_EXPORT D3D11MultiRenderTexture : public MultiRenderTexture
	{
	public:
		virtual ~D3D11MultiRenderTexture();

		bool requiresTextureFlipping() const { return false; }
		void getCustomAttribute(const String& name, void* pData);
	protected:
		friend class D3D11TextureManager;

		D3D11MultiRenderTexture();

		void setColorSurfaceImpl(UINT32 surfaceIdx, TexturePtr texture, UINT32 face = 0, UINT32 numFaces = 1, UINT32 mipLevel = 0);
		void setDepthStencilImpl(TexturePtr depthStencilSurface, UINT32 face = 0, UINT32 numFaces = 1, UINT32 mipLevel = 0);
	};
}
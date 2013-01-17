#pragma once

#include "CmD3D9Prerequisites.h"
#include "CmMultiRenderTexture.h"

namespace CamelotEngine
{
	class CM_D3D9_EXPORT D3D9MultiRenderTexture : public MultiRenderTexture
	{
	public:
		virtual ~D3D9MultiRenderTexture();

		bool requiresTextureFlipping() const { return false; }
		void getCustomAttribute(const String& name, void* pData);

	protected:
		friend class D3D9TextureManager;

		D3D9MultiRenderTexture();
		void initialize();

		void setColorSurfaceImpl(UINT32 surfaceIdx, TexturePtr texture, UINT32 face = 0, UINT32 numFaces = 1, UINT32 mipLevel = 0);
		void setDepthStencilImpl(TexturePtr depthStencilSurface, UINT32 face = 0, UINT32 numFaces = 1, UINT32 mipLevel = 0);

		vector<IDirect3DSurface9*>::type mDX9ColorSurfaces;
		IDirect3DSurface9* mDX9DepthStencilSurface;
	};
}
#pragma once

#include "CmD3D9Prerequisites.h"
#include "CmRenderTexture.h"

namespace CamelotEngine
{
	/// RenderTexture implementation for D3D9
	class CM_D3D9_EXPORT D3D9RenderTexture : public RenderTexture
	{
	public:
		D3D9RenderTexture();
		virtual ~D3D9RenderTexture();

		bool requiresTextureFlipping() const { return false; }
		virtual void getCustomAttribute_internal(const String& name, void* pData);

	protected:
		IDirect3DSurface9* mColorSurface;
		IDirect3DSurface9* mDepthStencilSurface;

		void createInternalResourcesImpl();
	};
}
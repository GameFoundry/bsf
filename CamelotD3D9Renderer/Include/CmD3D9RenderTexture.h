#pragma once

#include "CmD3D9Prerequisites.h"
#include "CmRenderTexture.h"

namespace CamelotFramework
{
	/// RenderTexture implementation for D3D9
	class CM_D3D9_EXPORT D3D9RenderTexture : public RenderTexture
	{
	public:
		virtual ~D3D9RenderTexture();

		bool requiresTextureFlipping() const { return false; }
		virtual void getCustomAttribute(const String& name, void* pData) const;

	protected:
		friend class D3D9TextureManager;

		D3D9RenderTexture();

		/**
		 * @copydoc RenderTexture::initialize_internal().
		 */
		void initialize_internal();

		IDirect3DSurface9* mDX9ColorSurface;
		IDirect3DSurface9* mDX9DepthStencilSurface;
		bool mIsBindableToShader;
	};
}
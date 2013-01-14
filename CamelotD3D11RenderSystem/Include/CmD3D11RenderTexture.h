#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmTexture.h"
#include "CmRenderTexture.h"

namespace CamelotEngine
{
	class D3D11RenderTexture : public RenderTexture
	{
	public:
		virtual ~D3D11RenderTexture();

		bool requiresTextureFlipping() const { return false; }
		void getCustomAttribute(const String& name, void* pData);

		static ID3D11RenderTargetView* createRenderTargetView(const SurfaceDesc& surfaceDesc);
	protected:
		friend class D3D11TextureManager;

		ID3D11RenderTargetView* mRenderTargetView;

		D3D11RenderTexture();
		void createInternalResourcesImpl();
	};
}
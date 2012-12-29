#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmTexture.h"
#include "CmRenderTexture.h"

namespace CamelotEngine
{
	class D3D11RenderTexture : public RenderTexture
	{
	public:
		D3D11RenderTexture();
		virtual ~D3D11RenderTexture();

		bool requiresTextureFlipping() const { return false; }

	protected:
		ID3D11RenderTargetView* mRenderTargetView;

		void createInternalResourcesImpl();
	};
}
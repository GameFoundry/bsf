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

	protected:
		friend class D3D11TextureManager;

		D3D11RenderTexture();

		/**
		 * @copydoc RenderTexture::initialize_internal().
		 */
		void initialize_internal();
	};
}
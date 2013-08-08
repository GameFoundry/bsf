/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#pragma once

#include "CmPrerequisites.h"
#include "CmTexture.h"
#include "CmRenderTarget.h"

namespace CamelotFramework
{    
	struct CM_EXPORT RENDER_TEXTURE_DESC
	{
		RENDER_SURFACE_DESC colorSurface;
		RENDER_SURFACE_DESC depthStencilSurface;
	};

    class CM_EXPORT RenderTexture : public RenderTarget
    {
	public:
		virtual ~RenderTexture();

		static RenderTexturePtr create(TextureType textureType, UINT32 width, UINT32 height, 
			PixelFormat format = PF_R8G8B8A8, bool hwGamma = false, UINT32 fsaa = 0, const String& fsaaHint = "", 
			bool createDepth = true, PixelFormat depthStencilFormat = PF_D24S8);

		void initialize(const RENDER_TEXTURE_DESC& desc);

		/**
		 * @copydoc RenderTarget::isWindow.
		 */
		bool isWindow() const { return false; }

		/**
		 * @copydoc RenderTarget::requiresTextureFlipping.
		 */
		bool requiresTextureFlipping() const { return false; }

		const HTexture& getBindableColorTexture() const { return mBindableColorTex; }
		const HTexture& getBindableDepthStencilTexture() const { return mBindableDepthStencilTex; }
	protected:
		TextureViewPtr mColorSurface;
		TextureViewPtr mDepthStencilSurface;

		HTexture mBindableColorTex;
		HTexture mBindableDepthStencilTex;

		RenderTexture();

		/**
		 * @copydoc RenderTarget::destroy_internal()
		 */
		virtual void destroy_internal();
	private:
		void throwIfBuffersDontMatch() const;

		virtual void copyToMemory(const PixelData &dst, FrameBuffer buffer = FB_AUTO);
	};
}
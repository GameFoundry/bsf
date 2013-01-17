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
#ifndef __RenderTexture_H__
#define __RenderTexture_H__

#include "CmPrerequisites.h"
#include "CmTexture.h"
#include "CmRenderTarget.h"

namespace CamelotEngine
{    
	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup RenderSystem
	*  @{
	*/
	/** This class represents a RenderTarget that renders to a Texture. There is no 1 on 1
        relation between Textures and RenderTextures, as there can be multiple 
        RenderTargets rendering to different mipmaps, faces (for cubemaps) or slices (for 3D textures)
        of the same Texture.
    */
    class CM_EXPORT RenderTexture : public RenderTarget
    {
	public:
		virtual ~RenderTexture();

		void setColorSurface(TexturePtr texture, UINT32 face = 0, UINT32 numFaces = 1, UINT32 mipLevel = 0);
		void setDepthStencilSurface(TexturePtr depthStencil, UINT32 face = 0, UINT32 numFaces = 0, UINT32 mipLevel = 0);

		bool requiresTextureFlipping() const { return false; }

	protected:
		TextureView* mColorSurface;
		TextureView* mDepthStencilSurface;

		RenderTexture();

		void createInternalResources();
		virtual void createInternalResourcesImpl() = 0;

	private:
		void throwIfBuffersDontMatch() const;

		virtual void copyContentsToMemory( const PixelData &dst, FrameBuffer buffer = FB_AUTO );
	};
	/** @} */
	/** @} */
}

#endif

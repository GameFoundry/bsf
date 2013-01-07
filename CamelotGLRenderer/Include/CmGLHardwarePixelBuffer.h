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
#ifndef __GLPIXELBUFFER_H__
#define __GLPIXELBUFFER_H__

#include "CmGLPrerequisites.h"
#include "CmHardwarePixelBuffer.h"

namespace CamelotEngine {
	class CM_RSGL_EXPORT GLHardwarePixelBuffer: public HardwarePixelBuffer
	{
	protected:  
		/// Lock a box
		PixelData lockImpl(const Box lockBox,  GpuLockOptions options);

		/// Unlock a box
		void unlockImpl(void);
        
		// Internal buffer; either on-card or in system memory, freed/allocated on demand
		// depending on buffer usage
		PixelData mBuffer;
        GLenum mGLInternalFormat; // GL internal format
		GpuLockOptions mCurrentLockOptions;
		
		// Buffer allocation/freeage
		void allocateBuffer();
		void freeBuffer();
		// Upload a box of pixels to this buffer on the card
		virtual void upload(const PixelData &data, const Box &dest);
		// Download a box of pixels from the card
		virtual void download(const PixelData &data);
	public:
        /// Should be called by HardwareBufferManager
        GLHardwarePixelBuffer(UINT32 mWidth, UINT32 mHeight, UINT32 mDepth,
                PixelFormat mFormat,
                GpuBufferUsage usage);
		
		/// @copydoc HardwarePixelBuffer::blitFromMemory
		void blitFromMemory(const PixelData &src, const Box &dstBox);
		
		/// @copydoc HardwarePixelBuffer::blitToMemory
		void blitToMemory(const Box &srcBox, const PixelData &dst);
		
		~GLHardwarePixelBuffer();
        
        /** Bind surface to frame buffer. Needs FBO extension.
        */
        virtual void bindToFramebuffer(GLenum attachment, UINT32 zoffset);
        GLenum getGLFormat() { return mGLInternalFormat; }
	};

    /** Texture surface.
    */
    class CM_RSGL_EXPORT GLTextureBuffer: public GLHardwarePixelBuffer
	{
    public:
        /** Texture constructor */
		GLTextureBuffer(const String &baseName, GLenum target, GLuint id, GLint face, 
			GLint level, GpuBufferUsage usage, bool softwareMipmap, bool writeGamma, UINT32 fsaa);
        ~GLTextureBuffer();
        
        /// @copydoc HardwarePixelBuffer::bindToFramebuffer
        virtual void bindToFramebuffer(GLenum attachment, UINT32 zoffset);
        /// Upload a box of pixels to this buffer on the card
		virtual void upload(const PixelData &data, const Box &dest);
		// Download a box of pixels from the card
		virtual void download(const PixelData &data);
  
        /// Hardware implementation of blitFromMemory
        virtual void blitFromMemory(const PixelData &src_orig, const Box &dstBox);
        
        /// Copy from framebuffer
        void copyFromFramebuffer(UINT32 zoffset);
        /// @copydoc HardwarePixelBuffer::blit
        void blit(const HardwarePixelBufferPtr &src, const Box &srcBox, const Box &dstBox);
        // Blitting implementation
        void blitFromTexture(GLTextureBuffer *src, const Box &srcBox, const Box &dstBox);
    protected:
        // In case this is a texture level
		GLenum mTarget;
		GLenum mFaceTarget; // same as mTarget in case of GL_TEXTURE_xD, but cubemap face for cubemaps
		GLuint mTextureID;
		GLint mFace;
		GLint mLevel;
		bool mSoftwareMipmap;		// Use GLU for mip mapping
    };
     /** Renderbuffer surface.  Needs FBO extension.
     */
    class CM_RSGL_EXPORT GLRenderBuffer: public GLHardwarePixelBuffer
	{
    public:
        GLRenderBuffer(GLenum format, UINT32 width, UINT32 height, GLsizei numSamples);
        ~GLRenderBuffer();
        
        /// @copydoc GLHardwarePixelBuffer::bindToFramebuffer
        virtual void bindToFramebuffer(GLenum attachment, UINT32 zoffset);
    protected:
        // In case this is a render buffer
        GLuint mRenderbufferID;
    };
};

#endif

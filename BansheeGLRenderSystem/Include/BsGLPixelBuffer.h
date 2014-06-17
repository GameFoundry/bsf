#pragma once

#include "BsGLPrerequisites.h"
#include "BsPixelBuffer.h"

namespace BansheeEngine 
{
	class GLTextureBuffer;

	class BS_RSGL_EXPORT GLPixelBuffer: public PixelBuffer
	{
	public:
		// Upload a box of pixels to this buffer on the card
		virtual void upload(const PixelData &data, const PixelVolume &dest);
		// Download a box of pixels from the card
		virtual void download(const PixelData &data);

		virtual void blitFromTexture(GLTextureBuffer *src);
		virtual void blitFromTexture(GLTextureBuffer *src, const PixelVolume &srcBox, const PixelVolume &dstBox);

	protected:  
		/// Lock a box
		PixelData lockImpl(PixelVolume lockBox,  GpuLockOptions options);

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
	public:
        /// Should be called by HardwareBufferManager
        GLPixelBuffer(UINT32 mWidth, UINT32 mHeight, UINT32 mDepth,
                PixelFormat mFormat,
                GpuBufferUsage usage);
		
		~GLPixelBuffer();
        
        /** Bind surface to frame buffer. Needs FBO extension.
        */
        virtual void bindToFramebuffer(GLenum attachment, UINT32 zoffset);
        GLenum getGLFormat() { return mGLInternalFormat; }
	};

    /** Texture surface.
    */
    class BS_RSGL_EXPORT GLTextureBuffer: public GLPixelBuffer
	{
    public:
        /** Texture constructor */
		GLTextureBuffer(const String &baseName, GLenum target, GLuint id, GLint face, 
			GLint level, GpuBufferUsage usage, bool softwareMipmap, bool writeGamma, UINT32 multisampleCount);
        ~GLTextureBuffer();
        
        /// @copydoc HardwarePixelBuffer::bindToFramebuffer
        virtual void bindToFramebuffer(GLenum attachment, UINT32 zoffset);
        /// Upload a box of pixels to this buffer on the card
		virtual void upload(const PixelData &data, const PixelVolume &dest);
		// Download a box of pixels from the card
		virtual void download(const PixelData &data);
  
        /// Copy from framebuffer
        void copyFromFramebuffer(UINT32 zoffset);

		void blitFromTexture(GLTextureBuffer *src, const PixelVolume &srcBox, const PixelVolume &dstBox);
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
    class BS_RSGL_EXPORT GLRenderBuffer: public GLPixelBuffer
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
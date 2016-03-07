//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsGLPrerequisites.h"
#include "BsPixelBuffer.h"

namespace BansheeEngine 
{
	/** @addtogroup GL
	 *  @{
	 */

	class GLTextureBuffer;

	/** OpenGL implementation of a pixel buffer. Represents a hardware buffer containing a surface of pixels. */
	class BS_RSGL_EXPORT GLPixelBuffer : public PixelBuffer
	{
	public:
		/**
		 * Constructs a new pixel buffer with the provided settings.
		 *
		 * @param[in]	width			Width of the pixel buffer in pixels.
		 * @param[in]	height			Height of the pixel buffer in pixels.
		 * @param[in]	depth			Depth of the pixel buffer in pixels (number of 2D slices).
		 * @param[in]	format			Format of each pixel in the buffer.
		 * @param[in]	usage			Usage signaling the render system how we plan on using the buffer.
		 */
		GLPixelBuffer(UINT32 mWidth, UINT32 mHeight, UINT32 mDepth,
			PixelFormat mFormat, GpuBufferUsage usage);

		~GLPixelBuffer();

		/**
		 * Upload some pixel data to the buffer.
		 *
		 * @param[in]	data	Data to upload.
		 * @param[in]	dest	Coordinates to which to upload the data.
		 */
		virtual void upload(const PixelData& data, const PixelVolume& dest);

		/**
		 * Reads data from the pixel buffer into the provided object. Caller must ensure the data object is of adequate
		 * size.
		 */
		virtual void download(const PixelData& data);

		/**
		 * Binds the buffers to a frame buffer object at the specified attachment point.
		 *
		 * @param[in]	attachment	Attachment point index in the range [0, BS_MAX_MULTIPLE_RENDER_TARGETS).
		 * @param[in]	zoffset		Depth slice to bind, in the case of a 3D texture.
		 */
		virtual void bindToFramebuffer(GLenum attachment, UINT32 zoffset);

		/**	Returns internal OpenGL pixel format used by the buffer. */
		GLenum getGLFormat() const { return mGLInternalFormat; }

		/**
		 * Blits the contents of the provided buffer into this pixel buffer. Data is bilinearily interpolated in case buffer
		 * sizes don't match.
		 */
		virtual void blitFromTexture(GLTextureBuffer* src);

		/**
		 * Blits contents of a sub-region of the provided buffer into a sub-region of this pixel buffer. Data is bilinearily
		 * interpolated in case source and destination sizes don't match.
		 */
		virtual void blitFromTexture(GLTextureBuffer* src, const PixelVolume& srcBox, const PixelVolume& dstBox);

	protected:  
		/** @copydoc PixelBuffer::lockImpl */
		PixelData lockImpl(PixelVolume lockBox, GpuLockOptions options) override;

		/** @copydoc PixelBuffer::unlockImpl */
		void unlockImpl() override;

		/**	Allocates an internal buffer on the CPU, the size of the hardware buffer. */
		void allocateBuffer();

		/**	Deallocates the internal CPU buffer. */
		void freeBuffer();

	protected:
		PixelData mBuffer;
		GLenum mGLInternalFormat;
		GpuLockOptions mCurrentLockOptions;
	};

	/**	Pixel buffer specialization that represents a single surface in a texture. */
    class BS_RSGL_EXPORT GLTextureBuffer : public GLPixelBuffer
	{
    public:
		/**
		 * Constructs a new texture buffer from a specific surface in the provided texture.
		 *
		 * @param[in]	target				OpenGL type of the texture to retrieve the surface from.
		 * @param[in]	id					OpenGL handle to the texture to retrieve the surface from.
		 * @param[in]	face				Face index of the texture in the case of cube textures or texture arrays.
		 * @param[in]	level				Mip level of the texture.
		 * @param[in]	usage				Usage signaling the render system how we plan on using the buffer.
		 * @param[in]	writeGamma			True if the parent texture was created with SRGB support.
		 * @param[in]	multisampleCount	Number of samples the parent texture was created with.
		 */
		GLTextureBuffer(GLenum target, GLuint id, GLint face, 
			GLint level, GpuBufferUsage usage, bool writeGamma, UINT32 multisampleCount);
        ~GLTextureBuffer();
        
		/** @copydoc GLPixelBuffer::bindToFramebuffer */
        void bindToFramebuffer(GLenum attachment, UINT32 zoffset) override;

		/** @copydoc GLPixelBuffer::upload */
		void upload(const PixelData &data, const PixelVolume &dest) override;

		/** @copydoc GLPixelBuffer::download */
		void download(const PixelData &data) override;
  
		/** @copydoc GLPixelBuffer::blitFromTexture */
		void blitFromTexture(GLTextureBuffer *src, const PixelVolume &srcBox, const PixelVolume &dstBox) override;

        /**
         * Populate texture buffer with the data in the currently attached frame buffer.
		 *
		 * @param[in]	zoffset		3D slice of the texture to copy to. 0 if texture is not 3D.
         */
        void copyFromFramebuffer(UINT32 zoffset);

    protected:
		GLenum mTarget;
		GLenum mFaceTarget;
		GLuint mTextureID;
		GLint mFace;
		GLint mLevel;
		UINT32 mMultisampleCount;
    };

	/**	Pixel buffer specialization that represents a render buffer. */
    class BS_RSGL_EXPORT GLRenderBuffer : public GLPixelBuffer
	{
    public:
		/**
		 * Initializes a new render buffer.
		 *
		 * @param[in]	format		OpenGL pixel format.
		 * @param[in]	width		Width of the render buffer in pixels.
		 * @param[in]	height		Height of the render buffer in pixels.
		 * @param[in]	numSamples	Number of samples to support.
		 */
        GLRenderBuffer(GLenum format, UINT32 width, UINT32 height, GLsizei numSamples);
        ~GLRenderBuffer();
        
		/** @copydoc	GLPixelBuffer::bindToFramebuffer */
        void bindToFramebuffer(GLenum attachment, UINT32 zoffset) override;
    protected:
        GLuint mRenderbufferID;
    };

	/** @} */
};
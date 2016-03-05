//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsGLPrerequisites.h"
#include "BsPixelBuffer.h"

namespace BansheeEngine 
{
	class GLTextureBuffer;

	/**
	 * @brief	OpenGL implementation of a pixel buffer. Represents a hardware buffer
	 *			containing a surface of pixels.
	 */
	class BS_RSGL_EXPORT GLPixelBuffer : public PixelBuffer
	{
	public:
		/**
		 * @brief	Constructs a new pixel buffer with the provided settings.
		 *
		 * @param	width			Width of the pixel buffer in pixels.
		 * @param	height			Height of the pixel buffer in pixels.
		 * @param	depth			Depth of the pixel buffer in pixels (number of 2D slices).
		 * @param	format			Format of each pixel in the buffer.
		 * @param	usage			Usage signaling the render system how we plan on using the buffer.
		 */
		GLPixelBuffer(UINT32 mWidth, UINT32 mHeight, UINT32 mDepth,
			PixelFormat mFormat, GpuBufferUsage usage);

		~GLPixelBuffer();

		/**
		 * @brief	Upload some pixel data to the buffer.
		 *
		 * @param	data	Data to upload.
		 * @param	dest	Coordinates to which to upload the data.
		 */
		virtual void upload(const PixelData& data, const PixelVolume& dest);

		/**
		 * @brief	Reads data from the pixel buffer into the provided object.
		 *			Caller must ensure the data object is of adequate size.
		 */
		virtual void download(const PixelData& data);

		/**
		 * @brief	Binds the buffers to a frame buffer object at the specified attachment point.
		 *
		 * @param	attachment	Attachment point index in the range [0, BS_MAX_MULTIPLE_RENDER_TARGETS).
		 * @param	zoffset		Depth slice to bind, in the case of a 3D texture.
		 */
		virtual void bindToFramebuffer(GLenum attachment, UINT32 zoffset);

		/**
		 * @brief	Returns internal OpenGL pixel format used by the buffer.
		 */
		GLenum getGLFormat() { return mGLInternalFormat; }

		/**
		 * @brief	Blits the contents of the provided buffer into this pixel buffer.
		 *			Data is bilinearily interpolated in case buffer sizes don't match.
		 */
		virtual void blitFromTexture(GLTextureBuffer* src);

		/**
		 * @brief	Blits contents of a sub-region of the provided buffer into a sub-region of this pixel buffer.
		 *			Data is bilinearily interpolated in case source and destination sizes don't match.
		 */
		virtual void blitFromTexture(GLTextureBuffer* src, const PixelVolume& srcBox, const PixelVolume& dstBox);

	protected:  
		/**
		 * @copydoc	PixelBuffer::lockImpl
		 */
		PixelData lockImpl(PixelVolume lockBox, GpuLockOptions options);

		/**
		 * @copydoc	PixelBuffer::unlockImpl
		 */
		void unlockImpl();

		/**
		 * @brief	Allocates an internal buffer on the CPU, the size of the hardware buffer.
		 */
		void allocateBuffer();

		/**
		 * @brief	Deallocates the internal CPU buffer.
		 */
		void freeBuffer();

	protected:
		PixelData mBuffer;
		GLenum mGLInternalFormat;
		GpuLockOptions mCurrentLockOptions;
	};

	/**
	 * @brief	Pixel buffer specialization that represents a single surface in a texture.
	 */
    class BS_RSGL_EXPORT GLTextureBuffer : public GLPixelBuffer
	{
    public:
		/**
		 * @brief	Constructs a new texture buffer from a specific surface in the provided texture.
		 *
		 * @param	target				OpenGL type of the texture to retrieve the surface from.
		 * @param	id					OpenGL handle to the texture to retrieve the surface from.
		 * @param	face				Face index of the texture in the case of cube textures or texture arrays.
		 * @param	level				Mip level of the texture.
		 * @param	usage				Usage signaling the render system how we plan on using the buffer.
		 * @param	writeGamma			True if the parent texture was created with SRGB support.
		 * @param	multisampleCount	Number of samples the parent texture was created with.
		 */
		GLTextureBuffer(GLenum target, GLuint id, GLint face, 
			GLint level, GpuBufferUsage usage, bool writeGamma, UINT32 multisampleCount);
        ~GLTextureBuffer();
        
		/**
		 * @copydoc	GLPixelBuffer::bindToFramebuffer
		 */
        virtual void bindToFramebuffer(GLenum attachment, UINT32 zoffset);

		/**
		 * @copydoc	GLPixelBuffer::upload
		 */
		virtual void upload(const PixelData &data, const PixelVolume &dest);

		/**
		 * @copydoc	GLPixelBuffer::download
		 */
		virtual void download(const PixelData &data);
  
		/**
		 * @copydoc	GLPixelBuffer::blitFromTexture
		 */
		void blitFromTexture(GLTextureBuffer *src, const PixelVolume &srcBox, const PixelVolume &dstBox);

        /**
         * @brief	Populate texture buffer with the data in the currently attached frame buffer.
		 *
		 * @param	zoffset		3D slice of the texture to copy to. 0 if texture is not 3D.
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

	/**
	 * @brief	Pixel buffer specialization that represents a render buffer.
	 */
    class BS_RSGL_EXPORT GLRenderBuffer : public GLPixelBuffer
	{
    public:
		/**
		 * @brief	Initializes a new render buffer.
		 *
		 * @param	format		OpenGL pixel format.
		 * @param	width		Width of the render buffer in pixels.
		 * @param	height		Height of the render buffer in pixels.
		 * @param	numSamples	Number of samples to support.
		 */
        GLRenderBuffer(GLenum format, UINT32 width, UINT32 height, GLsizei numSamples);
        ~GLRenderBuffer();
        
		/**
		 * @copydoc	GLPixelBuffer::bindToFramebuffer
		 */
        virtual void bindToFramebuffer(GLenum attachment, UINT32 zoffset);
    protected:
        GLuint mRenderbufferID;
    };
};
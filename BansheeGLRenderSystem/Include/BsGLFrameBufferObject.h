#pragma once

#include "BsGLPrerequisites.h"
#include "BsGLContext.h"
#include "BsGLPixelBuffer.h"
#include "BsPixelData.h"

namespace BansheeEngine 
{
	/** GL surface descriptor. Points to a 2D surface that can be rendered to. 
    */
    struct BS_RSGL_EXPORT GLSurfaceDesc
    {
    public:
        GLPixelBufferPtr buffer;
        UINT32 zoffset;
		UINT32 numSamples;

		GLSurfaceDesc() :buffer(0), zoffset(0), numSamples(0) {}
    };

    /** Frame Buffer Object abstraction.
    */
    class BS_RSGL_EXPORT GLFrameBufferObject
    {
    public:
        GLFrameBufferObject(UINT32 multisampleCount);
        ~GLFrameBufferObject();

        /** Bind a surface to a certain attachment point.
            attachment: 0..BS_MAX_MULTIPLE_RENDER_TARGETS-1
        */
        void bindSurface(UINT32 attachment, const GLSurfaceDesc &target);

        /** Unbind attachment
        */
        void unbindSurface(UINT32 attachment);

		/**
		 * @brief	Bind depth stencil buffer.
		 */
		void bindDepthStencil(GLPixelBufferPtr depthStencilBuffer);

		/**
		 * @brief	Unbinds depth stencil buffer.
		 */
		void unbindDepthStencil();
        
        /** Bind FrameBufferObject
        */
        void bind();

		/// Get the GL id for the FBO
		GLuint getGLFBOID() const { return mFB; }

        /// Accessors
        UINT32 getWidth();
        UINT32 getHeight();
        PixelFormat getFormat();
        
		const GLSurfaceDesc &getSurface(UINT32 attachment) { return mColor[attachment]; }
    private:
		GLsizei mNumSamples;
        GLuint mFB;

        GLPixelBufferPtr mDepthStencilBuffer;
        // Arbitrary number of texture surfaces
        GLSurfaceDesc mColor[BS_MAX_MULTIPLE_RENDER_TARGETS];

		/** Initialise object (find suitable depth and stencil format).
            Must be called every time the bindings change.
            It fails with an exception (ERR_INVALIDPARAMS) if:
            - Attachment point 0 has no binding
            - Not all bound surfaces have the same size
            - Not all bound surfaces have the same internal format
        */
        void initialize();
    };
}
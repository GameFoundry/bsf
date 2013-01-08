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
#ifndef __OgreGLFBO_H__
#define __OgreGLFBO_H__

#include "CmGLPrerequisites.h"
#include "CmGLContext.h"
#include "CmGLPixelBuffer.h"
#include "CmPixelData.h"

namespace CamelotEngine 
{
	/** GL surface descriptor. Points to a 2D surface that can be rendered to. 
    */
    struct CM_RSGL_EXPORT GLSurfaceDesc
    {
    public:
        GLHardwarePixelBufferPtr buffer;
        UINT32 zoffset;
		UINT32 numSamples;

		GLSurfaceDesc() :buffer(0), zoffset(0), numSamples(0) {}
    };

    /** Frame Buffer Object abstraction.
    */
    class CM_RSGL_EXPORT GLFrameBufferObject
    {
    public:
        GLFrameBufferObject(UINT32 fsaa);
        ~GLFrameBufferObject();

        /** Bind a surface to a certain attachment point.
            attachment: 0..CM_MAX_MULTIPLE_RENDER_TARGETS-1
        */
        void bindSurface(UINT32 attachment, const GLSurfaceDesc &target);

        /** Unbind attachment
        */
        void unbindSurface(UINT32 attachment);

		/**
		 * @brief	Bind depth stencil buffer.
		 */
		void bindDepthStencil(GLHardwarePixelBufferPtr depthStencilBuffer);

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

        GLHardwarePixelBufferPtr mDepthStencilBuffer;
        // Arbitrary number of texture surfaces
        GLSurfaceDesc mColor[CM_MAX_MULTIPLE_RENDER_TARGETS];

		/** Initialise object (find suitable depth and stencil format).
            Must be called every time the bindings change.
            It fails with an exception (ERR_INVALIDPARAMS) if:
            - Attachment point 0 has no binding
            - Not all bound surfaces have the same size
            - Not all bound surfaces have the same internal format
        */
        void initialise();
    };
}

#endif

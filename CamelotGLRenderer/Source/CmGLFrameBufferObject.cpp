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

#include "CmGLFrameBufferObject.h"
#include "CmGLPixelFormat.h"
#include "CmGLPixelBuffer.h"
#include "CmGLRenderTexture.h"

namespace BansheeEngine 
{
    GLFrameBufferObject::GLFrameBufferObject(UINT32 fsaa)
		:mNumSamples(fsaa)
    {
        /// Generate framebuffer object
        glGenFramebuffersEXT(1, &mFB);
		// check multisampling
		if (GLEW_EXT_framebuffer_blit && GLEW_EXT_framebuffer_multisample)
		{
			// check samples supported
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFB);
			GLint maxSamples;
			glGetIntegerv(GL_MAX_SAMPLES_EXT, &maxSamples);
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
			mNumSamples = std::min(mNumSamples, (GLsizei)maxSamples);
		}
		else
		{
			mNumSamples = 0;
		}

        /// Initialise state
        for(UINT32 x = 0; x < CM_MAX_MULTIPLE_RENDER_TARGETS; ++x)
            mColor[x].buffer = nullptr;
    }

    GLFrameBufferObject::~GLFrameBufferObject()
    {
        /// Delete framebuffer object
        glDeleteFramebuffersEXT(1, &mFB);        
    }

    void GLFrameBufferObject::bindSurface(UINT32 attachment, const GLSurfaceDesc &target)
    {
        assert(attachment < CM_MAX_MULTIPLE_RENDER_TARGETS);
        mColor[attachment] = target;

		// Re-initialise
		if(mColor[0].buffer)
			initialise();
    }

    void GLFrameBufferObject::unbindSurface(UINT32 attachment)
    {
        assert(attachment < CM_MAX_MULTIPLE_RENDER_TARGETS);
        mColor[attachment].buffer = nullptr;

		// Re-initialise if buffer 0 still bound
		if(mColor[0].buffer)
		{
			initialise();
		}
    }

	void GLFrameBufferObject::bindDepthStencil(GLPixelBufferPtr depthStencilBuffer)
	{
		mDepthStencilBuffer = depthStencilBuffer;
	}

	void GLFrameBufferObject::unbindDepthStencil()
	{
		mDepthStencilBuffer = nullptr;
	}

    void GLFrameBufferObject::initialise()
    {
        /// First buffer must be bound
        if(!mColor[0].buffer)
			CM_EXCEPT(InvalidParametersException, "Attachment 0 must have surface attached");

        /// Store basic stats
        UINT32 width = mColor[0].buffer->getWidth();
        UINT32 height = mColor[0].buffer->getHeight();
        GLuint glformat = mColor[0].buffer->getGLFormat();
        PixelFormat format = mColor[0].buffer->getFormat();
        UINT16 maxSupportedMRTs = BansheeEngine::RenderSystem::instancePtr()->getCapabilities()->getNumMultiRenderTargets();

		// Bind simple buffer to add colour attachments
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFB);

        /// Bind all attachment points to frame buffer
        for(UINT16 x = 0; x < maxSupportedMRTs; ++x)
        {
            if(mColor[x].buffer)
            {
                if(mColor[x].buffer->getWidth() != width || mColor[x].buffer->getHeight() != height)
                {
                    StringStream ss;
                    ss << "Attachment " << x << " has incompatible size ";
                    ss << mColor[x].buffer->getWidth() << "x" << mColor[x].buffer->getHeight();
                    ss << ". It must be of the same as the size of surface 0, ";
                    ss << width << "x" << height;
                    ss << ".";
                    CM_EXCEPT(InvalidParametersException, ss.str());
                }

                if(mColor[x].buffer->getGLFormat() != glformat)
                {
                    StringStream ss;
                    ss << "Attachment " << x << " has incompatible format.";
                    CM_EXCEPT(InvalidParametersException, ss.str());
                }

	            mColor[x].buffer->bindToFramebuffer(GL_COLOR_ATTACHMENT0_EXT+x, mColor[x].zoffset);
            }
            else
            {
                // Detach
                glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT+x,
                    GL_RENDERBUFFER_EXT, 0);
            }
        }

		if(mDepthStencilBuffer != nullptr)
			mDepthStencilBuffer->bindToFramebuffer(GL_DEPTH_STENCIL_ATTACHMENT, 0);

		/// Do glDrawBuffer calls
		GLenum bufs[CM_MAX_MULTIPLE_RENDER_TARGETS];
		GLsizei n=0;
		for(UINT32 x=0; x<CM_MAX_MULTIPLE_RENDER_TARGETS; ++x)
		{
			// Fill attached colour buffers
			if(mColor[x].buffer)
			{
				bufs[x] = GL_COLOR_ATTACHMENT0_EXT + x;
				// Keep highest used buffer + 1
				n = x+1;
			}
			else
			{
				bufs[x] = GL_NONE;
			}
		}

		if(glDrawBuffers)
		{
			/// Drawbuffer extension supported, use it
			glDrawBuffers(n, bufs);
		}
		else
		{
			/// In this case, the capabilities will not show more than 1 simultaneaous render target.
			glDrawBuffer(bufs[0]);
		}

		/// No read buffer, by default, if we want to read anyway we must not forget to set this.
		glReadBuffer(GL_NONE);

        /// Check status
        GLuint status;
        status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
        
        /// Bind main buffer
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        
        switch(status)
        {
        case GL_FRAMEBUFFER_COMPLETE_EXT:
            // All is good
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
            CM_EXCEPT(InvalidParametersException, 
            "All framebuffer formats with this texture internal format unsupported");
        default:
            CM_EXCEPT(InvalidParametersException, 
            "Framebuffer incomplete or other FBO status error");
        }
    }

    void GLFrameBufferObject::bind()
    {
        /// Bind it to FBO
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFB);
    }

    UINT32 GLFrameBufferObject::getWidth()
    {
        assert(mColor[0].buffer);
        return mColor[0].buffer->getWidth();
    }

    UINT32 GLFrameBufferObject::getHeight()
    {
        assert(mColor[0].buffer);
        return mColor[0].buffer->getHeight();
    }

    PixelFormat GLFrameBufferObject::getFormat()
    {
        assert(mColor[0].buffer);
        return mColor[0].buffer->getFormat();
    }
}

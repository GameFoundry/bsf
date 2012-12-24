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
#include "CmGLHardwarePixelBuffer.h"
#include "CmGLRenderTexture.h"

namespace CamelotEngine {

//-----------------------------------------------------------------------------
    GLFrameBufferObject::GLFrameBufferObject(GLRTTManager *manager, UINT32 fsaa):
        mManager(manager), mNumSamples(fsaa)
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
		// will we need a second FBO to do multisampling?
		if (mNumSamples)
		{
			glGenFramebuffersEXT(1, &mMultisampleFB);
		}
		else
		{
			mMultisampleFB = 0;
		}
        /// Initialise state
        mDepth.buffer=0;
        mStencil.buffer=0;
        for(UINT32 x=0; x<CM_MAX_MULTIPLE_RENDER_TARGETS; ++x)
        {
            mColour[x].buffer=0;
        }
    }
    GLFrameBufferObject::~GLFrameBufferObject()
    {
        mManager->releaseRenderBuffer(mDepth);
        mManager->releaseRenderBuffer(mStencil);
		mManager->releaseRenderBuffer(mMultisampleColourBuffer);
        /// Delete framebuffer object
        glDeleteFramebuffersEXT(1, &mFB);        
		if (mMultisampleFB)
			glDeleteFramebuffersEXT(1, &mMultisampleFB);

    }
    void GLFrameBufferObject::bindSurface(UINT32 attachment, const GLSurfaceDesc &target)
    {
        assert(attachment < CM_MAX_MULTIPLE_RENDER_TARGETS);
        mColour[attachment] = target;
		// Re-initialise
		if(mColour[0].buffer)
			initialise();
    }
    void GLFrameBufferObject::unbindSurface(UINT32 attachment)
    {
        assert(attachment < CM_MAX_MULTIPLE_RENDER_TARGETS);
        mColour[attachment].buffer = 0;
		// Re-initialise if buffer 0 still bound
		if(mColour[0].buffer)
		{
			initialise();
		}
    }
    void GLFrameBufferObject::initialise()
    {
		// Release depth and stencil, if they were bound
        mManager->releaseRenderBuffer(mDepth);
        mManager->releaseRenderBuffer(mStencil);
		mManager->releaseRenderBuffer(mMultisampleColourBuffer);
        /// First buffer must be bound
        if(!mColour[0].buffer)
        {
			CM_EXCEPT(InvalidParametersException, 
            "Attachment 0 must have surface attached");
        }

		// If we're doing multisampling, then we need another FBO which contains a
		// renderbuffer which is set up to multisample, and we'll blit it to the final 
		// FBO afterwards to perform the multisample resolve. In that case, the 
		// mMultisampleFB is bound during rendering and is the one with a depth/stencil

        /// Store basic stats
        UINT32 width = mColour[0].buffer->getWidth();
        UINT32 height = mColour[0].buffer->getHeight();
        GLuint glformat = mColour[0].buffer->getGLFormat();
        PixelFormat format = mColour[0].buffer->getFormat();
        UINT16 maxSupportedMRTs = CamelotEngine::RenderSystem::instancePtr()->getCapabilities()->getNumMultiRenderTargets();

		// Bind simple buffer to add colour attachments
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFB);

        /// Bind all attachment points to frame buffer
        for(UINT16 x=0; x<maxSupportedMRTs; ++x)
        {
            if(mColour[x].buffer)
            {
                if(mColour[x].buffer->getWidth() != width || mColour[x].buffer->getHeight() != height)
                {
                    StringStream ss;
                    ss << "Attachment " << x << " has incompatible size ";
                    ss << mColour[x].buffer->getWidth() << "x" << mColour[x].buffer->getHeight();
                    ss << ". It must be of the same as the size of surface 0, ";
                    ss << width << "x" << height;
                    ss << ".";
                    CM_EXCEPT(InvalidParametersException, ss.str());
                }
                if(mColour[x].buffer->getGLFormat() != glformat)
                {
                    StringStream ss;
                    ss << "Attachment " << x << " has incompatible format.";
                    CM_EXCEPT(InvalidParametersException, ss.str());
                }
	            mColour[x].buffer->bindToFramebuffer(GL_COLOR_ATTACHMENT0_EXT+x, mColour[x].zoffset);
            }
            else
            {
                // Detach
                glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT+x,
                    GL_RENDERBUFFER_EXT, 0);
            }
        }

		// Now deal with depth / stencil
		if (mMultisampleFB)
		{
			// Bind multisample buffer
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mMultisampleFB);

			// Create AA render buffer (colour)
			// note, this can be shared too because we blit it to the final FBO
			// right after the render is finished
			mMultisampleColourBuffer = mManager->requestRenderBuffer(glformat, width, height, mNumSamples);

			// Attach it, because we won't be attaching below and non-multisample has
			// actually been attached to other FBO
			mMultisampleColourBuffer.buffer->bindToFramebuffer(GL_COLOR_ATTACHMENT0_EXT, 
				mMultisampleColourBuffer.zoffset);

			// depth & stencil will be dealt with below

		}

        /// Find suitable depth and stencil format that is compatible with colour format
        GLenum depthFormat, stencilFormat;
        mManager->getBestDepthStencil(format, &depthFormat, &stencilFormat);
        
        /// Request surfaces
        mDepth = mManager->requestRenderBuffer(depthFormat, width, height, mNumSamples);
		if (depthFormat == GL_DEPTH24_STENCIL8_EXT)
		{
			// bind same buffer to depth and stencil attachments
            mManager->requestRenderBuffer(mDepth);
			mStencil = mDepth;
		}
		else
		{
			// separate stencil
			mStencil = mManager->requestRenderBuffer(stencilFormat, width, height, mNumSamples);
		}
        
        /// Attach/detach surfaces
        if(mDepth.buffer)
        {
            mDepth.buffer->bindToFramebuffer(GL_DEPTH_ATTACHMENT_EXT, mDepth.zoffset);
        }
        else
        {
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
                GL_RENDERBUFFER_EXT, 0);
        }
        if(mStencil.buffer)
        {
            mStencil.buffer->bindToFramebuffer(GL_STENCIL_ATTACHMENT_EXT, mStencil.zoffset);
        }
        else
        {
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT,
                GL_RENDERBUFFER_EXT, 0);
        }

		/// Do glDrawBuffer calls
		GLenum bufs[CM_MAX_MULTIPLE_RENDER_TARGETS];
		GLsizei n=0;
		for(UINT32 x=0; x<CM_MAX_MULTIPLE_RENDER_TARGETS; ++x)
		{
			// Fill attached colour buffers
			if(mColour[x].buffer)
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
		if (mMultisampleFB)
		{
			// we need a read buffer because we'll be blitting to mFB
			glReadBuffer(bufs[0]);
		}
		else
		{
			/// No read buffer, by default, if we want to read anyway we must not forget to set this.
			glReadBuffer(GL_NONE);
		}
        
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
		if (mMultisampleFB)
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mMultisampleFB);
		else
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFB);
    }

	void GLFrameBufferObject::swapBuffers()
	{
		if (mMultisampleFB)
		{
			// blit from multisample buffer to final buffer, triggers resolve
			UINT32 width = mColour[0].buffer->getWidth();
			UINT32 height = mColour[0].buffer->getHeight();
			glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, mMultisampleFB);
			glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, mFB);
			glBlitFramebufferEXT(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		}
	}

    UINT32 GLFrameBufferObject::getWidth()
    {
        assert(mColour[0].buffer);
        return mColour[0].buffer->getWidth();
    }
    UINT32 GLFrameBufferObject::getHeight()
    {
        assert(mColour[0].buffer);
        return mColour[0].buffer->getHeight();
    }
    PixelFormat GLFrameBufferObject::getFormat()
    {
        assert(mColour[0].buffer);
        return mColour[0].buffer->getFormat();
    }
}

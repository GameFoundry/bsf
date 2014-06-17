#include "BsGLFrameBufferObject.h"
#include "BsGLPixelFormat.h"
#include "BsGLPixelBuffer.h"
#include "BsGLRenderTexture.h"

namespace BansheeEngine 
{
    GLFrameBufferObject::GLFrameBufferObject()
    {
        glGenFramebuffersEXT(1, &mFB);

        for(UINT32 x = 0; x < BS_MAX_MULTIPLE_RENDER_TARGETS; ++x)
            mColor[x].buffer = nullptr;
    }

    GLFrameBufferObject::~GLFrameBufferObject()
    {
        glDeleteFramebuffersEXT(1, &mFB);        
    }

    void GLFrameBufferObject::bindSurface(UINT32 attachment, const GLSurfaceDesc &target)
    {
        assert(attachment < BS_MAX_MULTIPLE_RENDER_TARGETS);
        mColor[attachment] = target;

		if(mColor[0].buffer)
			rebuild();
    }

    void GLFrameBufferObject::unbindSurface(UINT32 attachment)
    {
        assert(attachment < BS_MAX_MULTIPLE_RENDER_TARGETS);
        mColor[attachment].buffer = nullptr;

		if(mColor[0].buffer)
			rebuild();
    }

	void GLFrameBufferObject::bindDepthStencil(GLPixelBufferPtr depthStencilBuffer)
	{
		mDepthStencilBuffer = depthStencilBuffer;
	}

	void GLFrameBufferObject::unbindDepthStencil()
	{
		mDepthStencilBuffer = nullptr;
	}

    void GLFrameBufferObject::rebuild()
    {
        // First buffer must be bound
        if(!mColor[0].buffer)
			BS_EXCEPT(InvalidParametersException, "Attachment 0 must have surface attached");

        // Store basic stats
        UINT32 width = mColor[0].buffer->getWidth();
        UINT32 height = mColor[0].buffer->getHeight();
        GLuint glformat = mColor[0].buffer->getGLFormat();
        PixelFormat format = mColor[0].buffer->getFormat();
        UINT16 maxSupportedMRTs = BansheeEngine::RenderSystem::instancePtr()->getCapabilities()->getNumMultiRenderTargets();

		// Bind simple buffer to add color attachments
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFB);

        // Bind all attachment points to frame buffer
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
                    BS_EXCEPT(InvalidParametersException, ss.str());
                }

                if(mColor[x].buffer->getGLFormat() != glformat)
                {
                    StringStream ss;
                    ss << "Attachment " << x << " has incompatible format.";
                    BS_EXCEPT(InvalidParametersException, ss.str());
                }

	            mColor[x].buffer->bindToFramebuffer(GL_COLOR_ATTACHMENT0_EXT + x, mColor[x].zoffset);
            }
            else
            {
                // Detach
                glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + x, GL_RENDERBUFFER_EXT, 0);
            }
        }

		if(mDepthStencilBuffer != nullptr)
			mDepthStencilBuffer->bindToFramebuffer(GL_DEPTH_STENCIL_ATTACHMENT, 0);

		// Do glDrawBuffer calls
		GLenum bufs[BS_MAX_MULTIPLE_RENDER_TARGETS];
		GLsizei n=0;
		for(UINT32 x=0; x<BS_MAX_MULTIPLE_RENDER_TARGETS; ++x)
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
			// Drawbuffer extension supported, use it
			glDrawBuffers(n, bufs);
		}
		else
		{
			// In this case, the capabilities will not show more than 1 simultaneaous render target.
			glDrawBuffer(bufs[0]);
		}

		// No read buffer, by default, if we want to read anyway we must not forget to set this.
		glReadBuffer(GL_NONE);

        // Check status
        GLuint status;
        status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
        
        // Bind main buffer
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        
        switch(status)
        {
        case GL_FRAMEBUFFER_COMPLETE_EXT:
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
            BS_EXCEPT(InvalidParametersException, "All framebuffer formats with this texture internal format unsupported");
        default:
            BS_EXCEPT(InvalidParametersException, "Framebuffer incomplete or other FBO status error");
        }
    }

    void GLFrameBufferObject::bind()
    {
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFB);
    }
}

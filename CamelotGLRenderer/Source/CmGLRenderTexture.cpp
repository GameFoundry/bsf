/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org

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
#include "CmGLRenderTexture.h"
#include "CmGLPixelFormat.h"
#include "CmGLDepthStencilBuffer.h"
#include "CmGLPixelBuffer.h"

namespace CamelotEngine 
{
	GLRenderTexture::GLRenderTexture()
		:mFB(nullptr)
	{
	}

	GLRenderTexture::~GLRenderTexture()
	{
		if(mFB != nullptr)
			delete mFB;
	}

	void GLRenderTexture::createInternalResourcesImpl()
	{
		if(mFB != nullptr)
			delete mFB;

		mFB = new GLFrameBufferObject(mFSAA);

		GLSurfaceDesc surfaceDesc;
		surfaceDesc.numSamples = mFSAA;
		surfaceDesc.zoffset = 0;

		GLTexture* glTexture = static_cast<GLTexture*>(mSurface.texture.get());
		surfaceDesc.buffer = std::static_pointer_cast<GLPixelBuffer>(glTexture->getBuffer(mSurface.face, mSurface.mipLevel));

		mFB->bindSurface(0, surfaceDesc);

		GLDepthStencilBuffer* glDepthStencilBuffer = static_cast<GLDepthStencilBuffer*>(mDepthStencilBuffer.get());

		mFB->bindDepthStencil(glDepthStencilBuffer->getGLRenderBuffer());
	}

	void GLRenderTexture::getCustomAttribute(const String& name, void* pData)
	{
		if(name=="FBO")
		{
			*static_cast<GLFrameBufferObject **>(pData) = mFB;
		}
		else if (name == "GL_FBOID" || name == "GL_MULTISAMPLEFBOID")
		{
			*static_cast<GLuint*>(pData) = mFB->getGLFBOID();
		}
	}

/// Size of probe texture
#define PROBE_SIZE 16

/// Stencil and depth formats to be tried
static const GLenum stencilFormats[] =
{
    GL_NONE,                    // No stencil
    GL_STENCIL_INDEX1_EXT,
    GL_STENCIL_INDEX4_EXT,
    GL_STENCIL_INDEX8_EXT,
    GL_STENCIL_INDEX16_EXT
};
static const UINT32 stencilBits[] =
{
    0, 1, 4, 8, 16
};
#define STENCILFORMAT_COUNT (sizeof(stencilFormats)/sizeof(GLenum))

static const GLenum depthFormats[] =
{
    GL_NONE,
    GL_DEPTH_COMPONENT16,
    GL_DEPTH_COMPONENT24,    // Prefer 24 bit depth
    GL_DEPTH_COMPONENT32,
    GL_DEPTH24_STENCIL8_EXT // packed depth / stencil
};
static const UINT32 depthBits[] =
{
    0,16,24,32,24
};
#define DEPTHFORMAT_COUNT (sizeof(depthFormats)/sizeof(GLenum))

	GLRTTManager::GLRTTManager()
    {
		detectFBOFormats();

        glGenFramebuffersEXT(1, &mTempFBO);
    }

	GLRTTManager::~GLRTTManager()
	{
        glDeleteFramebuffersEXT(1, &mTempFBO);      
	}

    /** Try a certain FBO format, and return the status. Also sets mDepthRB and mStencilRB.
        @returns true    if this combo is supported
                 false   if this combo is not supported
    */
    GLuint GLRTTManager::_tryFormat(GLenum depthFormat, GLenum stencilFormat)
    {
        GLuint status, depthRB = 0, stencilRB = 0;
        bool failed = false; // flag on GL errors

        if(depthFormat != GL_NONE)
        {
            /// Generate depth renderbuffer
            glGenRenderbuffersEXT(1, &depthRB);
            /// Bind it to FBO
            glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthRB);
            
            /// Allocate storage for depth buffer
            glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, depthFormat,
                                PROBE_SIZE, PROBE_SIZE);
            
            /// Attach depth
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
                                    GL_RENDERBUFFER_EXT, depthRB);
        }

        if(stencilFormat != GL_NONE)
        {
            /// Generate stencil renderbuffer
            glGenRenderbuffersEXT(1, &stencilRB);
            /// Bind it to FBO
            glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, stencilRB);
            glGetError(); // NV hack
            /// Allocate storage for stencil buffer
            glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, stencilFormat,
                                PROBE_SIZE, PROBE_SIZE); 
            if(glGetError() != GL_NO_ERROR) // NV hack
                failed = true;
            /// Attach stencil
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT,
                            GL_RENDERBUFFER_EXT, stencilRB);
            if(glGetError() != GL_NO_ERROR) // NV hack
                failed = true;
        }
        
        status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
        /// If status is negative, clean up
        // Detach and destroy
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);
        if (depthRB)
            glDeleteRenderbuffersEXT(1, &depthRB);
        if (stencilRB)
            glDeleteRenderbuffersEXT(1, &stencilRB);
        
        return status == GL_FRAMEBUFFER_COMPLETE_EXT && !failed;
    }
    
    /** Try a certain packed depth/stencil format, and return the status.
        @returns true    if this combo is supported
                 false   if this combo is not supported
    */
    bool GLRTTManager::_tryPackedFormat(GLenum packedFormat)
    {
        GLuint packedRB = 0;
        bool failed = false; // flag on GL errors

        /// Generate renderbuffer
        glGenRenderbuffersEXT(1, &packedRB);

        /// Bind it to FBO
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, packedRB);

        /// Allocate storage for buffer
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, packedFormat, PROBE_SIZE, PROBE_SIZE);
        glGetError(); // NV hack

        /// Attach depth
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
            GL_RENDERBUFFER_EXT, packedRB);
        if(glGetError() != GL_NO_ERROR) // NV hack
            failed = true;

        /// Attach stencil
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT,
            GL_RENDERBUFFER_EXT, packedRB);
        if(glGetError() != GL_NO_ERROR) // NV hack
            failed = true;

        GLuint status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

        /// Detach and destroy
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);
        glDeleteRenderbuffersEXT(1, &packedRB);

        return status == GL_FRAMEBUFFER_COMPLETE_EXT && !failed;
    }

    /** Detect which internal formats are allowed as RTT
        Also detect what combinations of stencil and depth are allowed with this internal
        format.
    */
    void GLRTTManager::detectFBOFormats()
    {
        // Try all formats, and report which ones work as target
        GLuint fb = 0, tid = 0;
        GLint old_drawbuffer = 0, old_readbuffer = 0;
        GLenum target = GL_TEXTURE_2D;

        glGetIntegerv (GL_DRAW_BUFFER, &old_drawbuffer);
        glGetIntegerv (GL_READ_BUFFER, &old_readbuffer);

        for(size_t x=0; x<PF_COUNT; ++x)
        {
            mProps[x].valid = false;

			// Fetch GL format token
			GLenum fmt = GLPixelUtil::getGLInternalFormat((PixelFormat)x);
            if(fmt == GL_NONE && x!=0)
                continue;

			// No test for compressed formats
			if(PixelUtil::isCompressed((PixelFormat)x))
				continue;

            // Create and attach framebuffer
            glGenFramebuffersEXT(1, &fb);
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
            if (fmt!=GL_NONE)
            {
				// Create and attach texture
				glGenTextures(1, &tid);
				glBindTexture(target, tid);
				
                // Set some default parameters so it won't fail on NVidia cards         
				if (GLEW_VERSION_1_2)
					glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, 0);
                glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                            
				glTexImage2D(target, 0, fmt, PROBE_SIZE, PROBE_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                                target, tid, 0);
            }
			else
			{
				// Draw to nowhere -- stencil/depth only
				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);
			}
            // Check status
            GLuint status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

			// Ignore status in case of fmt==GL_NONE, because no implementation will accept
			// a buffer without *any* attachment. Buffers with only stencil and depth attachment
			// might still be supported, so we must continue probing.
            if(fmt == GL_NONE || status == GL_FRAMEBUFFER_COMPLETE_EXT)
            {
                mProps[x].valid = true;

                // For each depth/stencil formats
                for (UINT32 depth = 0; depth < DEPTHFORMAT_COUNT; ++depth)
                {
                    if (depthFormats[depth] != GL_DEPTH24_STENCIL8_EXT)
                    {
                        // General depth/stencil combination
                        for (UINT32 stencil = 0; stencil < STENCILFORMAT_COUNT; ++stencil)
                        {
                            if (_tryFormat(depthFormats[depth], stencilFormats[stencil]))
                            {
                                /// Add mode to allowed modes
                                FormatProperties::Mode mode;
                                mode.depth = depth;
                                mode.stencil = stencil;
                                mProps[x].modes.push_back(mode);
                            }
                        }
                    }
                    else
                    {
                        // Packed depth/stencil format
                        if (_tryPackedFormat(depthFormats[depth]))
                        {
                            /// Add mode to allowed modes
                            FormatProperties::Mode mode;
                            mode.depth = depth;
                            mode.stencil = 0;   // unuse
                            mProps[x].modes.push_back(mode);
                        }
                    }
                }
            }
            // Delete texture and framebuffer
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
            glDeleteFramebuffersEXT(1, &fb);
			
			glFinish();
			
            if (fmt!=GL_NONE)
                glDeleteTextures(1, &tid);
        }

        // It seems a bug in nVidia driver: glBindFramebufferEXT should restore
        // draw and read buffers, but in some unclear circumstances it won't.
        glDrawBuffer(old_drawbuffer);
        glReadBuffer(old_readbuffer);

		String fmtstring = "";
        for(size_t x=0; x<PF_COUNT; ++x)
        {
            if(mProps[x].valid)
                fmtstring += PixelUtil::getFormatName((PixelFormat)x)+" ";
        }
    }
    
    PixelFormat GLRTTManager::getSupportedAlternative(PixelFormat format)
    {
        if(checkFormat(format))
            return format;
        /// Find first alternative
        PixelComponentType pct = PixelUtil::getComponentType(format);
        switch(pct)
        {
        case PCT_BYTE: format = PF_A8R8G8B8; break;
        case PCT_SHORT: format = PF_SHORT_RGBA; break;
        case PCT_FLOAT16: format = PF_FLOAT16_RGBA; break;
        case PCT_FLOAT32: format = PF_FLOAT32_RGBA; break;
        case PCT_COUNT: break;
        }
        if(checkFormat(format))
            return format;
        /// If none at all, return to default
        return PF_A8R8G8B8;
    }
}


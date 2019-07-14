//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsGLRenderTexture.h"
#include "BsGLPixelFormat.h"
#include "BsGLPixelBuffer.h"
#include "RenderAPI/BsTextureView.h"

namespace bs
{
#define PROBE_SIZE 16

	static const GLenum depthFormats[] =
	{
		GL_NONE,
		GL_DEPTH_COMPONENT16,
		GL_DEPTH_COMPONENT32,
		GL_DEPTH24_STENCIL8,
		GL_DEPTH32F_STENCIL8
	};

#define DEPTHFORMAT_COUNT (sizeof(depthFormats)/sizeof(GLenum))

	GLRenderTexture::GLRenderTexture(const RENDER_TEXTURE_DESC& desc)
		:RenderTexture(desc), mProperties(desc, true)
	{

	}

	namespace ct
	{
	GLRenderTexture::GLRenderTexture(const RENDER_TEXTURE_DESC& desc, UINT32 deviceIdx)
		:RenderTexture(desc, deviceIdx), mProperties(desc, true), mFB(nullptr)
	{
		assert(deviceIdx == 0 && "Multiple GPUs not supported natively on OpenGL.");
	}

	GLRenderTexture::~GLRenderTexture()
	{
		if (mFB != nullptr)
			bs_delete(mFB);
	}

	void GLRenderTexture::initialize()
	{
		RenderTexture::initialize();

		if (mFB != nullptr)
			bs_delete(mFB);

		mFB = bs_new<GLFrameBufferObject>();

		for (size_t i = 0; i < BS_MAX_MULTIPLE_RENDER_TARGETS; i++)
		{
			if (mColorSurfaces[i] != nullptr)
			{
				GLTexture* glColorSurface = static_cast<GLTexture*>(mDesc.colorSurfaces[i].texture.get());
				GLSurfaceDesc surfaceDesc;
				surfaceDesc.numSamples = getProperties().multisampleCount;

				if (mColorSurfaces[i]->getNumArraySlices() == 1) // Binding a single texture layer
				{
					surfaceDesc.allLayers = glColorSurface->getProperties().getNumFaces() == 1;

					if (glColorSurface->getProperties().getTextureType() != TEX_TYPE_3D)
					{
						surfaceDesc.zoffset = 0;
						surfaceDesc.buffer = glColorSurface->getBuffer(mColorSurfaces[i]->getFirstArraySlice(),
							mColorSurfaces[i]->getMostDetailedMip());
					}
					else
					{
						surfaceDesc.zoffset = 0;
						surfaceDesc.buffer = glColorSurface->getBuffer(0, mColorSurfaces[i]->getMostDetailedMip());
					}
				}
				else // Binding an array of textures or a range of 3D texture slices
				{
					surfaceDesc.allLayers = true;

					if (glColorSurface->getProperties().getTextureType() != TEX_TYPE_3D)
					{
						if (mColorSurfaces[i]->getNumArraySlices() != glColorSurface->getProperties().getNumFaces())
						{
							BS_LOG(Warning, RenderBackend, "OpenGL doesn't support binding of arbitrary ranges for array "
								"textures. The entire range will be bound instead.");
						}

						surfaceDesc.zoffset = 0;
						surfaceDesc.buffer = glColorSurface->getBuffer(0, mColorSurfaces[i]->getMostDetailedMip());
					}
					else
					{
						surfaceDesc.zoffset = 0;
						surfaceDesc.buffer = glColorSurface->getBuffer(0, mColorSurfaces[i]->getMostDetailedMip());
					}
				}

				mFB->bindSurface((UINT32)i, surfaceDesc);
			}
			else
			{
				mFB->unbindSurface((UINT32)i);
			}
		}

		if (mDepthStencilSurface != nullptr && mDesc.depthStencilSurface.texture != nullptr)
		{
			GLTexture* glDepthStencilTexture = static_cast<GLTexture*>(mDesc.depthStencilSurface.texture.get());
			SPtr<GLPixelBuffer> depthStencilBuffer = nullptr;

			bool allLayers = true;
			if (mDepthStencilSurface->getNumArraySlices() == 1) // Binding a single texture layer
				allLayers = glDepthStencilTexture->getProperties().getNumFaces() == 1;

			if (glDepthStencilTexture->getProperties().getTextureType() != TEX_TYPE_3D)
			{
				UINT32 firstSlice = 0;
				if (!allLayers)
					firstSlice = mDepthStencilSurface->getFirstArraySlice();

				depthStencilBuffer = glDepthStencilTexture->getBuffer(firstSlice,
					mDepthStencilSurface->getMostDetailedMip());
			}

			mFB->bindDepthStencil(depthStencilBuffer, allLayers);
		}

		mFB->rebuild();
	}

	void GLRenderTexture::getCustomAttribute(const String& name, void* data) const
	{
		if(name=="FBO")
		{
			*static_cast<GLFrameBufferObject**>(data) = mFB;
		}
		else if (name == "GL_FBOID" || name == "GL_MULTISAMPLEFBOID")
		{
			*static_cast<GLuint*>(data) = mFB->getGLFBOID();
		}
	}

	GLRTTManager::GLRTTManager()
		:mBlitReadFBO(0), mBlitWriteFBO(0)
	{
		detectFBOFormats();
		
		glGenFramebuffers(1, &mBlitReadFBO);
		BS_CHECK_GL_ERROR();

		glGenFramebuffers(1, &mBlitWriteFBO);
		BS_CHECK_GL_ERROR();
	}

	GLRTTManager::~GLRTTManager()
	{
		glDeleteFramebuffers(1, &mBlitReadFBO);
		BS_CHECK_GL_ERROR();

		glDeleteFramebuffers(1, &mBlitWriteFBO);
		BS_CHECK_GL_ERROR();
	}

	bool GLRTTManager::_tryFormat(GLenum depthFormat, GLenum stencilFormat)
	{
		GLuint status, depthRB = 0, stencilRB = 0;
		bool failed = false;

		if (depthFormat != GL_NONE)
		{
			// Generate depth renderbuffer
			glGenRenderbuffers(1, &depthRB);
			BS_CHECK_GL_ERROR();

			// Bind it to FBO
			glBindRenderbuffer(GL_RENDERBUFFER, depthRB);
			BS_CHECK_GL_ERROR();

			// Allocate storage for depth buffer
			glRenderbufferStorage(GL_RENDERBUFFER, depthFormat, PROBE_SIZE, PROBE_SIZE);

			if (glGetError() != GL_NO_ERROR)
				failed = true;

			// Attach depth
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRB);

			if (glGetError() != GL_NO_ERROR)
				failed = true;
		}

		if (stencilFormat != GL_NONE)
		{
			// Generate stencil renderbuffer
			glGenRenderbuffers(1, &stencilRB);
			BS_CHECK_GL_ERROR();

			// Bind it to FBO
			glBindRenderbuffer(GL_RENDERBUFFER, stencilRB);
			BS_CHECK_GL_ERROR();

			// Allocate storage for stencil buffer
			glRenderbufferStorage(GL_RENDERBUFFER, stencilFormat, PROBE_SIZE, PROBE_SIZE);

			if (glGetError() != GL_NO_ERROR)
				failed = true;

			// Attach stencil
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencilRB);

			if (glGetError() != GL_NO_ERROR)
				failed = true;
		}

		status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		BS_CHECK_GL_ERROR();

		// Detach and destroy
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
		BS_CHECK_GL_ERROR();

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
		BS_CHECK_GL_ERROR();

		if (depthRB)
		{
			glDeleteRenderbuffers(1, &depthRB);
			BS_CHECK_GL_ERROR();
		}

		if (stencilRB)
		{
			glDeleteRenderbuffers(1, &stencilRB);
			BS_CHECK_GL_ERROR();
		}

		return status == GL_FRAMEBUFFER_COMPLETE && !failed;
	}

	bool GLRTTManager::_tryPackedFormat(GLenum packedFormat)
	{
		GLuint packedRB = 0;
		bool failed = false; // flag on GL errors

		// Generate renderbuffer
		glGenRenderbuffers(1, &packedRB);
		BS_CHECK_GL_ERROR();

		// Bind it to FBO
		glBindRenderbuffer(GL_RENDERBUFFER, packedRB);
		BS_CHECK_GL_ERROR();

		// Allocate storage for buffer
		glRenderbufferStorage(GL_RENDERBUFFER, packedFormat, PROBE_SIZE, PROBE_SIZE);

		if (glGetError() != GL_NO_ERROR)
			failed = true;

		// Attach depth
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_RENDERBUFFER, packedRB);

		if (glGetError() != GL_NO_ERROR)
			failed = true;

		// Attach stencil
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
			GL_RENDERBUFFER, packedRB);

		if (glGetError() != GL_NO_ERROR)
			failed = true;

		GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		BS_CHECK_GL_ERROR();

		// Detach and destroy
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
		BS_CHECK_GL_ERROR();

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
		BS_CHECK_GL_ERROR();

		glDeleteRenderbuffers(1, &packedRB);
		BS_CHECK_GL_ERROR();

		return status == GL_FRAMEBUFFER_COMPLETE && !failed;
	}

	void GLRTTManager::detectFBOFormats()
	{
		// Try all formats, and report which ones work as target
		GLuint fb = 0, tid = 0;
		GLint oldDrawbuffer = 0, oldReadbuffer = 0;
		GLenum target = GL_TEXTURE_2D;

		glGetIntegerv(GL_DRAW_BUFFER, &oldDrawbuffer);
		BS_CHECK_GL_ERROR();

		glGetIntegerv(GL_READ_BUFFER, &oldReadbuffer);
		BS_CHECK_GL_ERROR();

		for (UINT32 x = 0; x < PF_COUNT; ++x)
		{
			mProps[x].valid = false;

			// Fetch GL format token
			GLenum fmt = GLPixelUtil::getGLInternalFormat((PixelFormat)x);
			if (fmt == GL_NONE && x != 0)
				continue;

			// No test for compressed formats
			if(PixelUtil::isCompressed((PixelFormat)x))
				continue;

			// No test for unnormalized integer targets
			if (!PixelUtil::isNormalized((PixelFormat)x) && !PixelUtil::isFloatingPoint((PixelFormat)x))
				continue;

			// Create and attach framebuffer
			glGenFramebuffers(1, &fb);
			BS_CHECK_GL_ERROR();

			glBindFramebuffer(GL_FRAMEBUFFER, fb);
			BS_CHECK_GL_ERROR();

			if (fmt != GL_NONE && !PixelUtil::isDepth((PixelFormat)x))
			{
				// Create and attach texture
				glGenTextures(1, &tid);
				BS_CHECK_GL_ERROR();

				glBindTexture(target, tid);
				BS_CHECK_GL_ERROR();
				
				glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, 0);
				BS_CHECK_GL_ERROR();

				glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				BS_CHECK_GL_ERROR();

				glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				BS_CHECK_GL_ERROR();

				glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				BS_CHECK_GL_ERROR();

				glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				BS_CHECK_GL_ERROR();

				glTexImage2D(target, 0, fmt, PROBE_SIZE, PROBE_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
				BS_CHECK_GL_ERROR();

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, tid, 0);
				BS_CHECK_GL_ERROR();
			}
			else
			{
				// Draw to nowhere (stencil/depth only)
				glDrawBuffer(GL_NONE);
				BS_CHECK_GL_ERROR();

				glReadBuffer(GL_NONE);
				BS_CHECK_GL_ERROR();
			}

			// Check status
			GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			BS_CHECK_GL_ERROR();

			// Ignore status in case of fmt==GL_NONE, because no implementation will accept
			// a buffer without *any* attachment. Buffers with only stencil and depth attachment
			// might still be supported, so we must continue probing.
			if (fmt == GL_NONE || status == GL_FRAMEBUFFER_COMPLETE)
			{
				mProps[x].valid = true;

				// For each depth/stencil formats
				for (UINT32 depth = 0; depth < DEPTHFORMAT_COUNT; ++depth)
				{
					if (depthFormats[depth] != GL_DEPTH24_STENCIL8 && depthFormats[depth] != GL_DEPTH32F_STENCIL8)
					{
						if (_tryFormat(depthFormats[depth], GL_NONE))
						{
							/// Add mode to allowed modes
							FormatProperties::Mode mode;
							mode.depth = depth;
							mode.stencil = 0;
							mProps[x].modes.push_back(mode);
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
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			BS_CHECK_GL_ERROR();

			glDeleteFramebuffers(1, &fb);
			BS_CHECK_GL_ERROR();
			
			glFinish();
			BS_CHECK_GL_ERROR();
			
			if (fmt != GL_NONE)
			{
				glDeleteTextures(1, &tid);
				BS_CHECK_GL_ERROR();
			}
		}

		glDrawBuffer(oldDrawbuffer);
		BS_CHECK_GL_ERROR();

		glReadBuffer(oldReadbuffer);
		BS_CHECK_GL_ERROR();
	}

	PixelFormat GLRTTManager::getSupportedAlternative(PixelFormat format)
	{
		if (checkFormat(format))
			return format;

		// Find first alternative
		PixelComponentType pct = PixelUtil::getElementType(format);
		switch (pct)
		{
		case PCT_BYTE: format = PF_RGBA8; break;
		case PCT_FLOAT16: format = PF_RGBA16F; break;
		case PCT_FLOAT32: format = PF_RGBA32F; break;
		default: break;
		}

		if (checkFormat(format))
			return format;

		// If none at all, return to default
		return PF_RGBA8;
	}
	}
}

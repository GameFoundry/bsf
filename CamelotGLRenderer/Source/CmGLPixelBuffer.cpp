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
#include "CmGLPixelBuffer.h"
#include "CmGLTexture.h"
#include "CmGLSupport.h"
#include "CmGLPixelFormat.h"
#include "CmException.h"
#include "CmBitwise.h"
#include "CmGLRenderTexture.h"

namespace BansheeEngine 
{
	GLPixelBuffer::GLPixelBuffer(UINT32 inWidth, UINT32 inHeight, UINT32 inDepth,
					PixelFormat inFormat,
					GpuBufferUsage usage):
		  PixelBuffer(inWidth, inHeight, inDepth, inFormat, usage, false),
		  mBuffer(inWidth, inHeight, inDepth, inFormat),
		  mGLInternalFormat(GL_NONE)
	{
		mCurrentLockOptions = (GpuLockOptions)0;
	}

	GLPixelBuffer::~GLPixelBuffer()
	{
		// Force free buffer
		mBuffer.freeInternalBuffer();
	}

	void GLPixelBuffer::allocateBuffer()
	{
		if(mBuffer.getData())
			// Already allocated
			return;

		mBuffer.allocateInternalBuffer();
		// TODO: use PBO if we're HBU_DYNAMIC
	}

	void GLPixelBuffer::freeBuffer()
	{
		// Free buffer if we're STATIC to save memory
		if(mUsage & GBU_STATIC)
		{
			mBuffer.freeInternalBuffer();
		}
	}

	PixelData GLPixelBuffer::lockImpl(const PixelVolume lockBox,  GpuLockOptions options)
	{
		allocateBuffer();
		if(options != GBL_WRITE_ONLY_DISCARD) 
		{
			// Download the old contents of the texture
			download(mBuffer);
		}
		mCurrentLockOptions = options;
		mLockedBox = lockBox;
		return mBuffer.getSubVolume(lockBox);
	}

	void GLPixelBuffer::unlockImpl(void)
	{
		if (mCurrentLockOptions != GBL_READ_ONLY)
		{
			// From buffer to card, only upload if was locked for writing
			upload(mCurrentLock, mLockedBox);
		}
	
		freeBuffer();
	}

	void GLPixelBuffer::upload(const PixelData &data, const PixelVolume &dest)
	{
		CM_EXCEPT(RenderingAPIException, 
			"Upload not possible for this pixelbuffer type");
	}

	void GLPixelBuffer::download(const PixelData &data)
	{
		CM_EXCEPT(RenderingAPIException, "Download not possible for this pixelbuffer type");
	}

	void GLPixelBuffer::blitFromTexture(GLTextureBuffer *src)
	{
		blitFromTexture(src, 
			PixelVolume(0,0,0,src->getWidth(),src->getHeight(),src->getDepth()), 
			PixelVolume(0,0,0,mWidth,mHeight,mDepth)
			);
	}

	void GLPixelBuffer::blitFromTexture(GLTextureBuffer *src, const PixelVolume &srcBox, const PixelVolume &dstBox)
	{
		CM_EXCEPT(RenderingAPIException, "BlitFromTexture not possible for this pixelbuffer type");
	}

	void GLPixelBuffer::bindToFramebuffer(GLenum attachment, UINT32 zoffset)
	{
		CM_EXCEPT(RenderingAPIException, "Framebuffer bind not possible for this pixelbuffer type");
	}

	GLTextureBuffer::GLTextureBuffer(const String &baseName, GLenum target, GLuint id, 
									 GLint face, GLint level, GpuBufferUsage usage, bool crappyCard, 
									 bool writeGamma, UINT32 fsaa):
		GLPixelBuffer(0, 0, 0, PF_UNKNOWN, usage),
		mTarget(target), mFaceTarget(0), mTextureID(id), mFace(face), mLevel(level),
		mSoftwareMipmap(crappyCard)
	{
		// devise mWidth, mHeight and mDepth and mFormat
		GLint value = 0;
	
		glBindTexture( mTarget, mTextureID );
	
		// Get face identifier
		mFaceTarget = mTarget;
		if(mTarget == GL_TEXTURE_CUBE_MAP)
			mFaceTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X + face;
	
		// Get width
		glGetTexLevelParameteriv(mFaceTarget, level, GL_TEXTURE_WIDTH, &value);
		mWidth = value;
	
		// Get height
		if(target == GL_TEXTURE_1D)
			value = 1;	// Height always 1 for 1D textures
		else
			glGetTexLevelParameteriv(mFaceTarget, level, GL_TEXTURE_HEIGHT, &value);
		mHeight = value;
	
		// Get depth
		if(target != GL_TEXTURE_3D)
			value = 1; // Depth always 1 for non-3D textures
		else
			glGetTexLevelParameteriv(mFaceTarget, level, GL_TEXTURE_DEPTH, &value);
		mDepth = value;

		// Get format
		glGetTexLevelParameteriv(mFaceTarget, level, GL_TEXTURE_INTERNAL_FORMAT, &value);
		mGLInternalFormat = value;
		mFormat = GLPixelUtil::getClosestEngineFormat(value);
	
		// Default
		mRowPitch = mWidth;
		mSlicePitch = mHeight*mWidth;
		mSizeInBytes = PixelUtil::getMemorySize(mWidth, mHeight, mDepth, mFormat);
	
		// Set up pixel box
		mBuffer = PixelData(mWidth, mHeight, mDepth, mFormat);
	
		if(mWidth==0 || mHeight==0 || mDepth==0)
			/// We are invalid, do not allocate a buffer
			return;
	}

	GLTextureBuffer::~GLTextureBuffer()
	{ }

	void GLTextureBuffer::upload(const PixelData &data, const PixelVolume &dest)
	{
		if((mUsage & TU_RENDERTARGET) != 0)
			CM_EXCEPT(NotImplementedException, "Writing to render texture from CPU not supported.");

		glBindTexture( mTarget, mTextureID );
		if(PixelUtil::isCompressed(data.getFormat()))
		{
			if(data.getFormat() != mFormat || !data.isConsecutive())
				CM_EXCEPT(InvalidParametersException, 
				"Compressed images must be consecutive, in the source format");
			GLenum format = GLPixelUtil::getClosestGLInternalFormat(mFormat);
			// Data must be consecutive and at beginning of buffer as PixelStorei not allowed
			// for compressed formats
			switch(mTarget) {
				case GL_TEXTURE_1D:
					// some systems (e.g. old Apple) don't like compressed subimage calls
					// so prefer non-sub versions
					if (dest.left == 0)
					{
						glCompressedTexImage1D(GL_TEXTURE_1D, mLevel,
							format,
							dest.getWidth(),
							0,
							data.getConsecutiveSize(),
							data.getData());
					}
					else
					{
						glCompressedTexSubImage1D(GL_TEXTURE_1D, mLevel, 
							dest.left,
							dest.getWidth(),
							format, data.getConsecutiveSize(),
							data.getData());
					}
					break;
				case GL_TEXTURE_2D:
				case GL_TEXTURE_CUBE_MAP:
					// some systems (e.g. old Apple) don't like compressed subimage calls
					// so prefer non-sub versions
					if (dest.left == 0 && dest.top == 0)
					{
						glCompressedTexImage2D(mFaceTarget, mLevel,
							format,
							dest.getWidth(),
							dest.getHeight(),
							0,
							data.getConsecutiveSize(),
							data.getData());
					}
					else
					{
						glCompressedTexSubImage2D(mFaceTarget, mLevel, 
							dest.left, dest.top, 
							dest.getWidth(), dest.getHeight(),
							format, data.getConsecutiveSize(),
							data.getData());
					}
					break;
				case GL_TEXTURE_3D:
					// some systems (e.g. old Apple) don't like compressed subimage calls
					// so prefer non-sub versions
					if (dest.left == 0 && dest.top == 0 && dest.front == 0)
					{
						glCompressedTexImage3D(GL_TEXTURE_3D, mLevel,
							format,
							dest.getWidth(),
							dest.getHeight(),
							dest.getDepth(),
							0,
							data.getConsecutiveSize(),
							data.getData());
					}
					else
					{			
						glCompressedTexSubImage3D(GL_TEXTURE_3D, mLevel, 
							dest.left, dest.top, dest.front,
							dest.getWidth(), dest.getHeight(), dest.getDepth(),
							format, data.getConsecutiveSize(),
							data.getData());
					}
					break;
			}
		
		} 
		else
		{
			if(data.getWidth() != data.getRowPitch())
				glPixelStorei(GL_UNPACK_ROW_LENGTH, data.getRowPitch());
			if(data.getHeight()*data.getWidth() != data.getSlicePitch())
				glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, (data.getSlicePitch()/data.getWidth()));
			if(data.getLeft() > 0 || data.getTop() > 0 || data.getFront() > 0)
				glPixelStorei(GL_UNPACK_SKIP_PIXELS, data.getLeft() + data.getRowPitch() * data.getTop() + data.getSlicePitch() * data.getFront());
			if((data.getWidth()*PixelUtil::getNumElemBytes(data.getFormat())) & 3) {
				// Standard alignment of 4 is not right
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			}
			switch(mTarget) {
				case GL_TEXTURE_1D:
					glTexSubImage1D(GL_TEXTURE_1D, mLevel, 
						dest.left,
						dest.getWidth(),
						GLPixelUtil::getGLOriginFormat(data.getFormat()), GLPixelUtil::getGLOriginDataType(data.getFormat()),
						data.getData());
					break;
				case GL_TEXTURE_2D:
				case GL_TEXTURE_CUBE_MAP:
					glTexSubImage2D(mFaceTarget, mLevel, 
						dest.left, dest.top, 
						dest.getWidth(), dest.getHeight(),
						GLPixelUtil::getGLOriginFormat(data.getFormat()), GLPixelUtil::getGLOriginDataType(data.getFormat()),
						data.getData());
					break;
				case GL_TEXTURE_3D:
					glTexSubImage3D(
						GL_TEXTURE_3D, mLevel, 
						dest.left, dest.top, dest.front,
						dest.getWidth(), dest.getHeight(), dest.getDepth(),
						GLPixelUtil::getGLOriginFormat(data.getFormat()), GLPixelUtil::getGLOriginDataType(data.getFormat()),
						data.getData());
					break;
			}	
		}
		// Restore defaults
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		if (GLEW_VERSION_1_2)
			glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, 0);
		glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}

	void GLTextureBuffer::download(const PixelData &data)
	{
		if((mUsage & TU_RENDERTARGET) != 0)
			CM_EXCEPT(NotImplementedException, "Reading from render texture to CPU not supported."); // TODO: This needs to be implemented

		if((mUsage & TU_DEPTHSTENCIL) != 0)
			CM_EXCEPT(NotImplementedException, "Reading from depth stencil texture to CPU not supported."); // TODO: This needs to be implemented

		if(data.getWidth() != getWidth() ||
			data.getHeight() != getHeight() ||
			data.getDepth() != getDepth())
			CM_EXCEPT(InvalidParametersException, "only download of entire buffer is supported by GL");
		glBindTexture( mTarget, mTextureID );
		if(PixelUtil::isCompressed(data.getFormat()))
		{
			if(data.getFormat() != mFormat || !data.isConsecutive())
				CM_EXCEPT(InvalidParametersException, 
				"Compressed images must be consecutive, in the source format");
			// Data must be consecutive and at beginning of buffer as PixelStorei not allowed
			// for compressed formate
			glGetCompressedTexImage(mFaceTarget, mLevel, data.getData());
		} 
		else
		{
			if(data.getWidth() != data.getRowPitch())
				glPixelStorei(GL_PACK_ROW_LENGTH, data.getRowPitch());
			if(data.getHeight()*data.getWidth() != data.getSlicePitch())
				glPixelStorei(GL_PACK_IMAGE_HEIGHT, (data.getSlicePitch()/data.getWidth()));
			if(data.getLeft() > 0 || data.getTop() > 0 || data.getFront() > 0)
				glPixelStorei(GL_PACK_SKIP_PIXELS, data.getLeft() + data.getRowPitch() * data.getTop() + data.getSlicePitch() * data.getFront());
			if((data.getWidth()*PixelUtil::getNumElemBytes(data.getFormat())) & 3) {
				// Standard alignment of 4 is not right
				glPixelStorei(GL_PACK_ALIGNMENT, 1);
			}
			// We can only get the entire texture
			glGetTexImage(mFaceTarget, mLevel, 
				GLPixelUtil::getGLOriginFormat(data.getFormat()), GLPixelUtil::getGLOriginDataType(data.getFormat()),
				data.getData());
			// Restore defaults
			glPixelStorei(GL_PACK_ROW_LENGTH, 0);
			glPixelStorei(GL_PACK_IMAGE_HEIGHT, 0);
			glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
			glPixelStorei(GL_PACK_ALIGNMENT, 4);
		}
	}

	void GLTextureBuffer::bindToFramebuffer(GLenum attachment, UINT32 zoffset)
	{
		assert(zoffset < mDepth);

		switch(mTarget)
		{
		case GL_TEXTURE_1D:
			glFramebufferTexture1DEXT(GL_FRAMEBUFFER_EXT, attachment,
				mFaceTarget, mTextureID, mLevel);
			break;
		case GL_TEXTURE_2D:
		case GL_TEXTURE_CUBE_MAP:
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, attachment,
				mFaceTarget, mTextureID, mLevel);
			break;
		case GL_TEXTURE_2D_MULTISAMPLE:
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, attachment,
				mFaceTarget, mTextureID, 0);
			break;
		case GL_TEXTURE_3D:
			glFramebufferTexture3DEXT(GL_FRAMEBUFFER_EXT, attachment,
				mFaceTarget, mTextureID, mLevel, zoffset);
			break;
		}
	}

	void GLTextureBuffer::copyFromFramebuffer(UINT32 zoffset)
	{
		glBindTexture(mTarget, mTextureID);
		switch(mTarget)
		{
		case GL_TEXTURE_1D:
			glCopyTexSubImage1D(mFaceTarget, mLevel, 0, 0, 0, mWidth);
			break;
		case GL_TEXTURE_2D:
		case GL_TEXTURE_CUBE_MAP:
			glCopyTexSubImage2D(mFaceTarget, mLevel, 0, 0, 0, 0, mWidth, mHeight);
			break;
		case GL_TEXTURE_3D:
			glCopyTexSubImage3D(mFaceTarget, mLevel, 0, 0, zoffset, 0, 0, mWidth, mHeight);
			break;
		}
	}

	/// Very fast texture-to-texture blitter and hardware bi/trilinear scaling implementation using FBO
	/// Destination texture must be 1D, 2D, 3D, or Cube
	/// Source texture must be 1D, 2D or 3D
	/// Supports compressed formats as both source and destination format, it will use the hardware DXT compressor
	/// if available.
	/// @author W.J. van der Laan
	void GLTextureBuffer::blitFromTexture(GLTextureBuffer *src, const PixelVolume &srcBox, const PixelVolume &dstBox)
	{
		//std::cerr << "GLTextureBuffer::blitFromTexture " <<
		//src->mTextureID << ":" << srcBox.left << "," << srcBox.top << "," << srcBox.right << "," << srcBox.bottom << " " << 
		//mTextureID << ":" << dstBox.left << "," << dstBox.top << "," << dstBox.right << "," << dstBox.bottom << std::endl;
		/// Store reference to FBO manager
		GLRTTManager *fboMan = static_cast<GLRTTManager *>(GLRTTManager::instancePtr());

		/// Save and clear GL state for rendering
		glPushAttrib(GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | 
			GL_FOG_BIT | GL_LIGHTING_BIT | GL_POLYGON_BIT | GL_SCISSOR_BIT | GL_STENCIL_BUFFER_BIT |
			GL_TEXTURE_BIT | GL_VIEWPORT_BIT);

		// Important to disable all other texture units
		if (GLEW_VERSION_1_2)
		{
			glActiveTextureARB(GL_TEXTURE0);
		}


		/// Disable alpha, depth and scissor testing, disable blending, 
		/// disable culling, disble lighting, disable fog and reset foreground
		/// colour.
		glDisable(GL_ALPHA_TEST);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_SCISSOR_TEST);
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glDisable(GL_LIGHTING);
		glDisable(GL_FOG);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		/// Save and reset matrices
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glMatrixMode(GL_TEXTURE);
		glPushMatrix();
		glLoadIdentity();

		/// Set up source texture
		glBindTexture(src->mTarget, src->mTextureID);

		/// Set filtering modes depending on the dimensions and source
		if(srcBox.getWidth()==dstBox.getWidth() &&
			srcBox.getHeight()==dstBox.getHeight() &&
			srcBox.getDepth()==dstBox.getDepth())
		{
			/// Dimensions match -- use nearest filtering (fastest and pixel correct)
			glTexParameteri(src->mTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(src->mTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		else
		{
			/// Manual mipmaps, stay safe with bilinear filtering so that no
			/// intermipmap leakage occurs.
			glTexParameteri(src->mTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(src->mTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		/// Clamp to edge (fastest)
		glTexParameteri(src->mTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(src->mTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(src->mTarget, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		/// Set origin base level mipmap to make sure we source from the right mip
		/// level.
		glTexParameteri(src->mTarget, GL_TEXTURE_BASE_LEVEL, src->mLevel);

		/// Store old binding so it can be restored later
		GLint oldfb;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &oldfb);

		/// Set up temporary FBO
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboMan->getTemporaryFBO());

		GLuint tempTex = 0;
		if(!fboMan->checkFormat(mFormat))
		{
			/// If target format not directly supported, create intermediate texture
			GLenum tempFormat = GLPixelUtil::getClosestGLInternalFormat(fboMan->getSupportedAlternative(mFormat));
			glGenTextures(1, &tempTex);
			glBindTexture(GL_TEXTURE_2D, tempTex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
			/// Allocate temporary texture of the size of the destination area
			glTexImage2D(GL_TEXTURE_2D, 0, tempFormat, 
				GLPixelUtil::optionalPO2(dstBox.getWidth()), GLPixelUtil::optionalPO2(dstBox.getHeight()), 
				0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
				GL_TEXTURE_2D, tempTex, 0);
			/// Set viewport to size of destination slice
			glViewport(0, 0, dstBox.getWidth(), dstBox.getHeight());
		}
		else
		{
			/// We are going to bind directly, so set viewport to size and position of destination slice
			glViewport(dstBox.left, dstBox.top, dstBox.getWidth(), dstBox.getHeight());
		}

		/// Process each destination slice
		for(UINT32 slice=dstBox.front; slice<dstBox.back; ++slice)
		{
			if(!tempTex)
			{
				/// Bind directly
				bindToFramebuffer(GL_COLOR_ATTACHMENT0_EXT, slice);
			}
			/// Calculate source texture coordinates
			float u1 = (float)srcBox.left / (float)src->mWidth;
			float v1 = (float)srcBox.top / (float)src->mHeight;
			float u2 = (float)srcBox.right / (float)src->mWidth;
			float v2 = (float)srcBox.bottom / (float)src->mHeight;
			/// Calculate source slice for this destination slice
			float w = (float)(slice - dstBox.front) / (float)dstBox.getDepth();
			/// Get slice # in source
			w = w * (float)(srcBox.getDepth() + srcBox.front);
			/// Normalise to texture coordinate in 0.0 .. 1.0
			w = (w+0.5f) / (float)src->mDepth;

			/// Finally we're ready to rumble	
			glBindTexture(src->mTarget, src->mTextureID);
			glEnable(src->mTarget);
			glBegin(GL_QUADS);
			glTexCoord3f(u1, v1, w);
			glVertex2f(-1.0f, -1.0f);
			glTexCoord3f(u2, v1, w);
			glVertex2f(1.0f, -1.0f);
			glTexCoord3f(u2, v2, w);
			glVertex2f(1.0f, 1.0f);
			glTexCoord3f(u1, v2, w);
			glVertex2f(-1.0f, 1.0f);
			glEnd();
			glDisable(src->mTarget);

			if(tempTex)
			{
				/// Copy temporary texture
				glBindTexture(mTarget, mTextureID);
				switch(mTarget)
				{
				case GL_TEXTURE_1D:
					glCopyTexSubImage1D(mFaceTarget, mLevel, 
						dstBox.left, 
						0, 0, dstBox.getWidth());
					break;
				case GL_TEXTURE_2D:
				case GL_TEXTURE_CUBE_MAP:
					glCopyTexSubImage2D(mFaceTarget, mLevel, 
						dstBox.left, dstBox.top, 
						0, 0, dstBox.getWidth(), dstBox.getHeight());
					break;
				case GL_TEXTURE_3D:
					glCopyTexSubImage3D(mFaceTarget, mLevel, 
						dstBox.left, dstBox.top, slice, 
						0, 0, dstBox.getWidth(), dstBox.getHeight());
					break;
				}
			}
		}

		/// Reset source texture to sane state
		glBindTexture(src->mTarget, src->mTextureID);
		glTexParameteri(src->mTarget, GL_TEXTURE_BASE_LEVEL, 0);

		/// Detach texture from temporary framebuffer
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
			GL_RENDERBUFFER_EXT, 0);
		/// Restore old framebuffer
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, oldfb);
		/// Restore matrix stacks and render state
		glMatrixMode(GL_TEXTURE);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glPopAttrib();
		glDeleteTextures(1, &tempTex);
	}
	
	GLRenderBuffer::GLRenderBuffer(GLenum format, UINT32 width, UINT32 height, GLsizei numSamples):
		GLPixelBuffer(width, height, 1, GLPixelUtil::getClosestEngineFormat(format), GBU_DYNAMIC),
		mRenderbufferID(0)
	{
		mGLInternalFormat = format;
		/// Generate renderbuffer
		glGenRenderbuffersEXT(1, &mRenderbufferID);
		/// Bind it to FBO
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, mRenderbufferID);
    
		/// Allocate storage for depth buffer
		if (numSamples > 0)
		{
			glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, 
				numSamples, format, width, height);
		}
		else
		{
			glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, format,
								width, height);
		}
	}

	GLRenderBuffer::~GLRenderBuffer()
	{
		/// Generate renderbuffer
		glDeleteRenderbuffersEXT(1, &mRenderbufferID);
	}

	void GLRenderBuffer::bindToFramebuffer(GLenum attachment, UINT32 zoffset)
	{
		assert(zoffset < mDepth);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, attachment,
							GL_RENDERBUFFER_EXT, mRenderbufferID);
	}
};

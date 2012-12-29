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

#include "CmGLTexture.h"
#include "CmGLSupport.h"
#include "CmGLPixelFormat.h"
#include "CmGLHardwarePixelBuffer.h"

//#include "CmTextureManager.h"
#include "CmCamera.h"
#include "CmException.h"
#include "CmBitwise.h"
#include "CmTextureManager.h"

#include "CmGLRenderTexture.h"

#if CM_PLATFORM == CM_PLATFORM_WIN32
#  define WIN32_LEAN_AND_MEAN
#  if !defined(NOMINMAX) && defined(_MSC_VER)
#	define NOMINMAX // required to stop windows.h messing up std::min
#  endif
#  include <windows.h>
#  include <wingdi.h>
#endif

#if CM_DEBUG_MODE
#define THROW_IF_NOT_RENDER_THREAD throwIfNotRenderThread();
#else
#define THROW_IF_NOT_RENDER_THREAD 
#endif

namespace CamelotEngine {

    GLTexture::GLTexture(GLSupport& support) 
        : Texture(),
        mTextureID(0), mGLSupport(support)
    {
    }


    GLTexture::~GLTexture()
    {
		THROW_IF_NOT_RENDER_THREAD;

		freeInternalResources();
    }

	void GLTexture::initialize_internal()
	{
		createInternalResources();

		Resource::initialize_internal();
	}

    GLenum GLTexture::getGLTextureTarget(void) const
    {
        switch(mTextureType)
        {
            case TEX_TYPE_1D:
                return GL_TEXTURE_1D;
            case TEX_TYPE_2D:
				if(mFSAA > 0)
					return GL_TEXTURE_2D_MULTISAMPLE;
				else
					return GL_TEXTURE_2D;
            case TEX_TYPE_3D:
                return GL_TEXTURE_3D;
            case TEX_TYPE_CUBE_MAP:
                return GL_TEXTURE_CUBE_MAP;
            default:
                return 0;
        };
    }

	GLuint GLTexture::getGLID() const
	{
		THROW_IF_NOT_RENDER_THREAD;

		return mTextureID;
	}

	//* Creation / loading methods ********************************************
	PixelData GLTexture::lockImpl(LockOptions options, UINT32 mipLevel, UINT32 face)
	{
		if(mLockedBuffer != nullptr)
			CM_EXCEPT(InternalErrorException, "Trying to lock a buffer that's already locked.");

		UINT32 mipWidth = mipLevel >> mWidth;
		UINT32 mipHeight = mipLevel >> mHeight;
		UINT32 mipDepth = mipLevel >> mDepth;

		PixelData lockedArea(mipWidth, mipHeight, mipDepth, mFormat);

		mLockedBuffer = getBuffer(face, mipLevel);
		lockedArea.data = mLockedBuffer->lock(options);

		return lockedArea;
	}
	/****************************************************************************************/
	void GLTexture::unlockImpl()
	{
		if(mLockedBuffer == nullptr)
			CM_EXCEPT(InternalErrorException, "Trying to unlock a buffer that's not locked.");

		mLockedBuffer->unlock();
		mLockedBuffer = nullptr;
	}
	/****************************************************************************************/ 
	void GLTexture::copyImpl(TexturePtr& target)
	{
		size_t numMips = std::min(getNumMipmaps(), target->getNumMipmaps());

		GLTexture* glTexture = static_cast<GLTexture*>(target.get());
		for(unsigned int face=0; face<getNumFaces(); face++)
		{
			for(unsigned int mip=0; mip<=numMips; mip++)
			{
				glTexture->getBuffer(face, mip)->blit(getBuffer(face, mip));
			}
		}
	}
	/****************************************************************************************/
	void GLTexture::createInternalResourcesImpl(void)
    {
		if (!GLEW_VERSION_1_2 && mTextureType == TEX_TYPE_3D)
			CM_EXCEPT(NotImplementedException, 
				"3D Textures not supported before OpenGL 1.2");

		// Convert to nearest power-of-two size if required
        mWidth = GLPixelUtil::optionalPO2(mWidth);      
        mHeight = GLPixelUtil::optionalPO2(mHeight);
        mDepth = GLPixelUtil::optionalPO2(mDepth);

		// Adjust format if required
		mFormat = TextureManager::instance().getNativeFormat(mTextureType, mFormat, mUsage);
		
		// Check requested number of mipmaps
		UINT32 maxMips = GLPixelUtil::getMaxMipmaps(mWidth, mHeight, mDepth, mFormat);
		if(mNumMipmaps>maxMips)
			mNumMipmaps = maxMips;
		
		if((mUsage & TU_RENDERTARGET) != 0)
		{
			mNumMipmaps = 1;

			if(mTextureType != TEX_TYPE_2D)
				CM_EXCEPT(NotImplementedException, "Only 2D render targets are supported at the moment");
		}

		// Generate texture name
        glGenTextures( 1, &mTextureID );
		
		// Set texture type
		glBindTexture( getGLTextureTarget(), mTextureID );
        
		// This needs to be set otherwise the texture doesn't get rendered
		if (GLEW_VERSION_1_2)
			glTexParameteri( getGLTextureTarget(), GL_TEXTURE_MAX_LEVEL, mNumMipmaps );
        
        // Set some misc default parameters so NVidia won't complain, these can of course be changed later
        glTexParameteri(getGLTextureTarget(), GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(getGLTextureTarget(), GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		if (GLEW_VERSION_1_2)
		{
			glTexParameteri(getGLTextureTarget(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(getGLTextureTarget(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
		
		// Allocate internal buffer so that glTexSubImageXD can be used
		// Internal format
		GLenum format = GLPixelUtil::getClosestGLInternalFormat(mFormat, mHwGamma);
		UINT32 width = mWidth;
		UINT32 height = mHeight;
		UINT32 depth = mDepth;

		GLenum glFormat = GL_RGBA;
		if(PixelUtil::isCompressed(mFormat))
		{
			if((mUsage & TU_RENDERTARGET) != 0)
				CM_EXCEPT(InvalidParametersException, "Cannot use a compressed format for a render target.");

			glFormat = GLPixelUtil::getBaseFormatFromCompressedInternalFormat(mFormat);
		}

		if((mUsage & TU_RENDERTARGET) != 0 && mTextureType == TEX_TYPE_2D && mFSAA > 0)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D, mFSAA, format,
				width, height, GL_FALSE);
		}
		else
		{
			// Run through this process to pregenerate mipmap piramid
			for(UINT32 mip=0; mip<=mNumMipmaps; mip++)
			{
				// Normal formats
				switch(mTextureType)
				{
				case TEX_TYPE_1D:
					glTexImage1D(GL_TEXTURE_1D, mip, format,
						width, 0, 
						GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

					break;
				case TEX_TYPE_2D:
					glTexImage2D(GL_TEXTURE_2D, mip, format,
						width, height, 0, 
						GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
					break;
				case TEX_TYPE_3D:
					glTexImage3D(GL_TEXTURE_3D, mip, format,
						width, height, depth, 0, 
						GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
					break;
				case TEX_TYPE_CUBE_MAP:
					for(int face=0; face<6; face++) {
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, mip, format,
							width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
					}
					break;
				};
				if(width>1)		width = width/2;
				if(height>1)	height = height/2;
				if(depth>1)		depth = depth/2;
			}
		}

		createSurfaceList();
		// Get final internal format
		mFormat = getBuffer(0,0)->getFormat();
	}
	//--------------------------------------------------------------------------------------------
    void GLTexture::freeInternalResourcesImpl()
    {
		mSurfaceList.clear();
        glDeleteTextures( 1, &mTextureID );
    }
	//---------------------------------------------------------------------------------------------
	void GLTexture::createSurfaceList()
	{
		mSurfaceList.clear();
		
		for(UINT32 face=0; face<getNumFaces(); face++)
		{
			for(UINT32 mip=0; mip<=getNumMipmaps(); mip++)
			{
                GLHardwarePixelBuffer *buf = new GLTextureBuffer("", getGLTextureTarget(), mTextureID, face, mip,
						static_cast<HardwareBuffer::Usage>(mUsage), false, mHwGamma, mFSAA);
				mSurfaceList.push_back(HardwarePixelBufferPtr(buf));
                
                /// Check for error
                if(buf->getWidth()==0 || buf->getHeight()==0 || buf->getDepth()==0)
                {
					CM_EXCEPT(RenderingAPIException, 
                        "Zero sized texture surface on texture face "
						+ toString(face) 
						+ " mipmap "+toString(mip)
						+ ". Probably, the GL driver refused to create the texture.");
                }
			}
		}
	}
	
	//---------------------------------------------------------------------------------------------
	HardwarePixelBufferPtr GLTexture::getBuffer(UINT32 face, UINT32 mipmap)
	{
		THROW_IF_NOT_RENDER_THREAD;

		if(face >= getNumFaces())
			CM_EXCEPT(InvalidParametersException, "Face index out of range");
		if(mipmap > mNumMipmaps)
			CM_EXCEPT(InvalidParametersException, "Mipmap index out of range");
		unsigned int idx = face*(mNumMipmaps+1) + mipmap;
		assert(idx < mSurfaceList.size());
		return mSurfaceList[idx];
	}
	//---------------------------------------------------------------------------------------------
	void GLTexture::getCustomAttribute_internal(const String& name, void* pData)
	{
		THROW_IF_NOT_RENDER_THREAD;

		if (name == "GLID")
			*static_cast<GLuint*>(pData) = mTextureID;
	}
	
}

#undef THROW_IF_NOT_RENDER_THREAD
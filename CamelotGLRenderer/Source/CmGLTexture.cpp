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
#include "CmRenderSystemManager.h"

#include "CmGLFBORenderTexture.h"

#if CM_PLATFORM == CM_PLATFORM_WIN32
#  define WIN32_LEAN_AND_MEAN
#  if !defined(NOMINMAX) && defined(_MSC_VER)
#	define NOMINMAX // required to stop windows.h messing up std::min
#  endif
#  include <windows.h>
#  include <wingdi.h>
#endif

namespace CamelotEngine {



    GLTexture::GLTexture(GLSupport& support) 
        : Texture(),
        mTextureID(0), mGLSupport(support)
    {
    }


    GLTexture::~GLTexture()
    {
		freeInternalResources();
    }

    GLenum GLTexture::getGLTextureTarget(void) const
    {
        switch(mTextureType)
        {
            case TEX_TYPE_1D:
                return GL_TEXTURE_1D;
            case TEX_TYPE_2D:
                return GL_TEXTURE_2D;
            case TEX_TYPE_3D:
                return GL_TEXTURE_3D;
            case TEX_TYPE_CUBE_MAP:
                return GL_TEXTURE_CUBE_MAP;
            default:
                return 0;
        };
    }

	//* Creation / loading methods ********************************************
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
		size_t maxMips = GLPixelUtil::getMaxMipmaps(mWidth, mHeight, mDepth, mFormat);
		mNumMipmaps = mNumRequestedMipmaps;
		if(mNumMipmaps>maxMips)
			mNumMipmaps = maxMips;
		
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
		
		// If we can do automip generation and the user desires this, do so
		mMipmapsHardwareGenerated = 
			CamelotEngine::RenderSystemManager::getActive()->getCapabilities()->hasCapability(RSC_AUTOMIPMAP);
		// NVIDIA 175.16 drivers break hardware mip generation for non-compressed
		// textures - disable until fixed
		// Leave hardware gen on compressed textures since that's the only way we
		// can realistically do it since GLU doesn't support DXT
		// However DON'T do this on Apple, their drivers aren't subject to this
		// problem yet and in fact software generation appears to cause a crash 
		// in some cases which I've yet to track down
#if CM_PLATFORM != CM_PLATFORM_APPLE
		if (CamelotEngine::RenderSystemManager::getActive()->getCapabilities()->getVendor() == GPU_NVIDIA
			&& !PixelUtil::isCompressed(mFormat))
		{
			mMipmapsHardwareGenerated = false;
		}
#endif
		if((mUsage & TU_AUTOMIPMAP) &&
		    mNumRequestedMipmaps && mMipmapsHardwareGenerated)
        {
            glTexParameteri( getGLTextureTarget(), GL_GENERATE_MIPMAP, GL_TRUE );
        }
		
		// Allocate internal buffer so that glTexSubImageXD can be used
		// Internal format
		GLenum format = GLPixelUtil::getClosestGLInternalFormat(mFormat, mHwGamma);
		size_t width = mWidth;
		size_t height = mHeight;
		size_t depth = mDepth;

		if(PixelUtil::isCompressed(mFormat))
		{
			// Compressed formats
			size_t size = PixelUtil::getMemorySize(mWidth, mHeight, mDepth, mFormat);
			// Provide temporary buffer filled with zeroes as glCompressedTexImageXD does not
			// accept a 0 pointer like normal glTexImageXD
			// Run through this process for every mipmap to pregenerate mipmap piramid
			UINT8 *tmpdata = new UINT8[size];
			memset(tmpdata, 0, size);
			
			for(size_t mip=0; mip<=mNumMipmaps; mip++)
			{
				size = PixelUtil::getMemorySize(width, height, depth, mFormat);
				switch(mTextureType)
				{
					case TEX_TYPE_1D:
						glCompressedTexImage1DARB(GL_TEXTURE_1D, mip, format, 
							width, 0, 
							size, tmpdata);
						break;
					case TEX_TYPE_2D:
						glCompressedTexImage2DARB(GL_TEXTURE_2D, mip, format,
							width, height, 0, 
							size, tmpdata);
						break;
					case TEX_TYPE_3D:
						glCompressedTexImage3DARB(GL_TEXTURE_3D, mip, format,
							width, height, depth, 0, 
							size, tmpdata);
						break;
					case TEX_TYPE_CUBE_MAP:
						for(int face=0; face<6; face++) {
							glCompressedTexImage2DARB(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, mip, format,
								width, height, 0, 
								size, tmpdata);
						}
						break;
				};
				if(width>1)		width = width/2;
				if(height>1)	height = height/2;
				if(depth>1)		depth = depth/2;
			}
			delete [] tmpdata;
		}
		else
		{
			// Run through this process to pregenerate mipmap piramid
			for(size_t mip=0; mip<=mNumMipmaps; mip++)
			{
				// Normal formats
				switch(mTextureType)
				{
					case TEX_TYPE_1D:
						glTexImage1D(GL_TEXTURE_1D, mip, format,
							width, 0, 
							GL_RGBA, GL_UNSIGNED_BYTE, 0);
	
						break;
					case TEX_TYPE_2D:
						glTexImage2D(GL_TEXTURE_2D, mip, format,
							width, height, 0, 
							GL_RGBA, GL_UNSIGNED_BYTE, 0);
						break;
					case TEX_TYPE_3D:
						glTexImage3D(GL_TEXTURE_3D, mip, format,
							width, height, depth, 0, 
							GL_RGBA, GL_UNSIGNED_BYTE, 0);
						break;
					case TEX_TYPE_CUBE_MAP:
						for(int face=0; face<6; face++) {
							glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, mip, format,
								width, height, 0, 
								GL_RGBA, GL_UNSIGNED_BYTE, 0);
						}
						break;
				};
				if(width>1)		width = width/2;
				if(height>1)	height = height/2;
				if(depth>1)		depth = depth/2;
			}
		}
		_createSurfaceList();
		// Get final internal format
		mFormat = getBuffer(0,0)->getFormat();
	}
	
    void GLTexture::createRenderTexture(void)
    {
        // Create the GL texture
		// This already does everything neccessary
        createInternalResources();
    }

    void GLTexture::load()
    {
        if( mUsage & TU_RENDERTARGET )
        {
            createRenderTexture();
            return;
        }

		initializeFromTextureData();

		mTextureData.clear();
    }

	//*************************************************************************
    
    void GLTexture::freeInternalResourcesImpl()
    {
		mSurfaceList.clear();
        glDeleteTextures( 1, &mTextureID );
    }

	
	//---------------------------------------------------------------------------------------------
	void GLTexture::_createSurfaceList()
	{
		mSurfaceList.clear();
		
		// For all faces and mipmaps, store surfaces as HardwarePixelBufferPtr
		bool wantGeneratedMips = (mUsage & TU_AUTOMIPMAP)!=0;
		
		// Do mipmapping in software? (uses GLU) For some cards, this is still needed. Of course,
		// only when mipmap generation is desired.
		bool doSoftware = wantGeneratedMips && !mMipmapsHardwareGenerated && getNumMipmaps(); 
		
		for(size_t face=0; face<getNumFaces(); face++)
		{
			for(size_t mip=0; mip<=getNumMipmaps(); mip++)
			{
                GLHardwarePixelBuffer *buf = new GLTextureBuffer("", getGLTextureTarget(), mTextureID, face, mip,
						static_cast<HardwareBuffer::Usage>(mUsage), doSoftware && mip==0, mHwGamma, mFSAA);
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
	HardwarePixelBufferPtr GLTexture::getBuffer(size_t face, size_t mipmap)
	{
		if(face >= getNumFaces())
			CM_EXCEPT(InvalidParametersException, "Face index out of range");
		if(mipmap > mNumMipmaps)
			CM_EXCEPT(InvalidParametersException, "Mipmap index out of range");
		unsigned int idx = face*(mNumMipmaps+1) + mipmap;
		assert(idx < mSurfaceList.size());
		return mSurfaceList[idx];
	}
	//---------------------------------------------------------------------------------------------
	void GLTexture::getCustomAttribute(const String& name, void* pData)
	{
		if (name == "GLID")
			*static_cast<GLuint*>(pData) = mTextureID;
	}
	
}


#include "BsGLTexture.h"
#include "BsGLSupport.h"
#include "BsGLPixelFormat.h"
#include "BsGLPixelBuffer.h"

#include "BsException.h"
#include "BsBitwise.h"
#include "BsCoreThread.h"
#include "BsTextureManager.h"

#include "BsGLRenderTexture.h"

namespace BansheeEngine 
{
    GLTexture::GLTexture(GLSupport& support) 
        : Texture(),
        mTextureID(0), mGLSupport(support)
    {
    }

    GLTexture::~GLTexture()
    {
    }

	void GLTexture::initialize_internal()
	{
		// Check requested number of mipmaps
		UINT32 maxMips = PixelUtil::getMaxMipmaps(mWidth, mHeight, mDepth, mFormat);
		if(mNumMipmaps > maxMips)
			BS_EXCEPT(InvalidParametersException, "Invalid number of mipmaps. Maximum allowed is: " + toString(maxMips));

		if((mUsage & TU_RENDERTARGET) != 0)
		{
			if(mTextureType != TEX_TYPE_2D)
				BS_EXCEPT(NotImplementedException, "Only 2D render targets are supported at the moment");
		}

		if((mUsage & TU_DEPTHSTENCIL) != 0)
		{
			if(mTextureType != TEX_TYPE_2D)
				BS_EXCEPT(NotImplementedException, "Only 2D depth stencil targets are supported at the moment");

			if(!PixelUtil::isDepth(mFormat))
				BS_EXCEPT(NotImplementedException, "Supplied format is not a depth stencil format. Format: " + toString(mFormat));
		}

		// Generate texture name
		glGenTextures( 1, &mTextureID );

		// Set texture type
		glBindTexture( getGLTextureTarget(), mTextureID );

		// This needs to be set otherwise the texture doesn't get rendered
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

		if(PixelUtil::isCompressed(mFormat))
		{
			if((mUsage & TU_RENDERTARGET) != 0)
				BS_EXCEPT(InvalidParametersException, "Cannot use a compressed format for a render target.");

			if((mUsage & TU_DEPTHSTENCIL) != 0)
				BS_EXCEPT(InvalidParametersException, "Cannot use a compressed format for a depth stencil target.");
		}

		if((mUsage & TU_RENDERTARGET) != 0 && mTextureType == TEX_TYPE_2D && mMultisampleCount > 0)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D, mMultisampleCount, format,
				width, height, GL_FALSE);
		}
		else if((mUsage & TU_DEPTHSTENCIL) != 0)
		{
			if(mMultisampleCount > 0)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D, mMultisampleCount, format,
					width, height, GL_FALSE);
			}
			else
			{
				GLenum depthStencilFormat = GLPixelUtil::getDepthStencilTypeFromFormat(mFormat);

				glTexImage2D(GL_TEXTURE_2D, 0, format,
					width, height, 0, 
					GL_DEPTH_STENCIL, depthStencilFormat, nullptr);
			}
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

		PixelBufferPtr buffer = getBuffer(0, 0);

#if BS_DEBUG_MODE
		if(buffer != nullptr)
		{
			if(mFormat != buffer->getFormat())
			{
				BS_EXCEPT(InternalErrorException, "Could not create a texture buffer with wanted format: " + toString(mFormat));
			}
		}
#endif

		Texture::initialize_internal();
	}

	void GLTexture::destroy_internal()
	{
		mSurfaceList.clear();
		glDeleteTextures( 1, &mTextureID );

		clearBufferViews();

		Texture::destroy_internal();
	}

    GLenum GLTexture::getGLTextureTarget(void) const
    {
        switch(mTextureType)
        {
            case TEX_TYPE_1D:
                return GL_TEXTURE_1D;
            case TEX_TYPE_2D:
				if(mMultisampleCount > 0)
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
		THROW_IF_NOT_CORE_THREAD;

		return mTextureID;
	}

	PixelData GLTexture::lockImpl(GpuLockOptions options, UINT32 mipLevel, UINT32 face)
	{
		if(mLockedBuffer != nullptr)
			BS_EXCEPT(InternalErrorException, "Trying to lock a buffer that's already locked.");

		UINT32 mipWidth = mWidth >> mipLevel;
		UINT32 mipHeight = mHeight >> mipLevel;
		UINT32 mipDepth = mDepth >> mipLevel;

		PixelData lockedArea(mipWidth, mipHeight, mipDepth, mFormat);

		mLockedBuffer = getBuffer(face, mipLevel);
		lockedArea.setExternalBuffer((UINT8*)mLockedBuffer->lock(options));

		return lockedArea;
	}

	void GLTexture::unlockImpl()
	{
		if(mLockedBuffer == nullptr)
			BS_EXCEPT(InternalErrorException, "Trying to unlock a buffer that's not locked.");

		mLockedBuffer->unlock();
		mLockedBuffer = nullptr;
	}


	void GLTexture::readData(PixelData& dest, UINT32 mipLevel, UINT32 face)
	{
		getBuffer(face, mipLevel)->download(dest);
	}

	void GLTexture::writeData(const PixelData& src, UINT32 mipLevel, UINT32 face, bool discardWholeBuffer)
	{
		getBuffer(face, mipLevel)->upload(src, src.getExtents());
	}

	void GLTexture::copyImpl(TexturePtr& target)
	{
		size_t numMips = std::min(getNumMipmaps(), target->getNumMipmaps());

		GLTexture* glTexture = static_cast<GLTexture*>(target.get());
		for(unsigned int face=0; face<getNumFaces(); face++)
		{
			for(unsigned int mip=0; mip<=numMips; mip++)
			{
				GLTextureBuffer *src = static_cast<GLTextureBuffer*>(getBuffer(face, mip).get());

				glTexture->getBuffer(face, mip)->blitFromTexture(src);
			}
		}
	}

	void GLTexture::createSurfaceList()
	{
		mSurfaceList.clear();
		
		for(UINT32 face=0; face<getNumFaces(); face++)
		{
			for(UINT32 mip=0; mip<=getNumMipmaps(); mip++)
			{
                GLPixelBuffer *buf = bs_new<GLTextureBuffer, PoolAlloc>(getGLTextureTarget(), mTextureID, face, mip,
						static_cast<GpuBufferUsage>(mUsage), mHwGamma, mMultisampleCount);
				mSurfaceList.push_back(bs_shared_ptr<GLPixelBuffer, PoolAlloc>(buf));
                
                /// Check for error
                if(buf->getWidth()==0 || buf->getHeight()==0 || buf->getDepth()==0)
                {
					BS_EXCEPT(RenderingAPIException, 
                        "Zero sized texture surface on texture face "
						+ toString(face) 
						+ " mipmap "+toString(mip)
						+ ". Probably, the GL driver refused to create the texture.");
                }
			}
		}
	}
	
	std::shared_ptr<GLPixelBuffer> GLTexture::getBuffer(UINT32 face, UINT32 mipmap)
	{
		THROW_IF_NOT_CORE_THREAD;

		if(face >= getNumFaces())
			BS_EXCEPT(InvalidParametersException, "Face index out of range");
		if(mipmap > mNumMipmaps)
			BS_EXCEPT(InvalidParametersException, "Mipmap index out of range");
		unsigned int idx = face*(mNumMipmaps+1) + mipmap;
		assert(idx < mSurfaceList.size());
		return mSurfaceList[idx];
	}
}
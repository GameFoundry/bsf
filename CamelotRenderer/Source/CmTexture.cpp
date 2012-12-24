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
#include "CmHardwarePixelBuffer.h"
#include "CmTexture.h"
#include "CmTextureRTTI.h"
#include "CmDataStream.h"
#include "CmException.h"
#include "CmDebug.h"
#include "CmRenderSystem.h"
#include "CmAsyncOp.h"

#if CM_DEBUG_MODE
#define THROW_IF_NOT_RENDER_THREAD throwIfNotRenderThread();
#else
#define THROW_IF_NOT_RENDER_THREAD 
#endif

namespace CamelotEngine {
	//--------------------------------------------------------------------------
    Texture::Texture()
        : 
            mHeight(512),
            mWidth(512),
            mDepth(1),
            mNumMipmaps(0),
			mHwGamma(false),
			mFSAA(0),
            mTextureType(TEX_TYPE_2D),            
            mFormat(PF_UNKNOWN),
            mUsage(TU_DEFAULT)
    {
        
    }
	//-------------------------------------------------------------------------
	void Texture::initialize(TextureType textureType, UINT32 width, UINT32 height, UINT32 depth, UINT32 numMipmaps, 
		PixelFormat format, int usage, bool hwGamma, UINT32 fsaa, const String& fsaaHint)
	{
		mTextureType = textureType;
		mWidth = width;
		mHeight = height;
		mDepth = depth;
		mNumMipmaps = numMipmaps;
		mFormat = format;
		mUsage = usage;
		mHwGamma = hwGamma;
		mFSAA = fsaa;
		mFSAAHint = fsaaHint;

		mSize = getNumFaces() * PixelUtil::getMemorySize(mWidth, mHeight, mDepth, mFormat);

		RenderSystem::instancePtr()->queueCommand(boost::bind(&Texture::initialize_internal, this));
	}
    //--------------------------------------------------------------------------
    bool Texture::hasAlpha(void) const
    {
        return PixelUtil::hasAlpha(mFormat);
    }
    //--------------------------------------------------------------------------
	UINT32 Texture::calculateSize(void) const
	{
        return getNumFaces() * PixelUtil::getMemorySize(mWidth, mHeight, mDepth, mFormat);
	}
	//--------------------------------------------------------------------------
	UINT32 Texture::getNumFaces(void) const
	{
		return getTextureType() == TEX_TYPE_CUBE_MAP ? 6 : 1;
	}
	//-------------------------------------------------------------------------
	void Texture::getCustomAttribute_internal(const String& name, void* pData)
	{
		THROW_IF_NOT_RENDER_THREAD;
	}
	//-----------------------------------------------------------------------------
	void Texture::createInternalResources(void)
	{
		createInternalResourcesImpl();
	}
	//-----------------------------------------------------------------------------
	void Texture::freeInternalResources(void)
	{
		freeInternalResourcesImpl();
	}

	void Texture::setRawPixels(const PixelData& data, UINT32 face, UINT32 mip)
	{
		RenderSystem::instancePtr()->queueCommand(boost::bind(&Texture::setRawPixels_internal, this, data, face, mip), true);
	}

	void Texture::setRawPixels_async(const PixelData& data, UINT32 face, UINT32 mip)
	{
		RenderSystem::instancePtr()->queueCommand(boost::bind(&Texture::setRawPixels_internal, this, data, face, mip));
	}

	void Texture::setRawPixels_internal(const PixelData& data, UINT32 face, UINT32 mip)
	{
		THROW_IF_NOT_RENDER_THREAD

		if(mip < 0 || mip > mNumMipmaps)
			CM_EXCEPT(InvalidParametersException, "Invalid mip level: " + toString(mip) + ". Min is 0, max is " + toString(mNumMipmaps));

		if(face < 0 || mip > getNumFaces())
			CM_EXCEPT(InvalidParametersException, "Invalid face index: " + toString(face) + ". Min is 0, max is " + toString(getNumFaces()));

		if(mFormat != data.format)
			CM_EXCEPT(InvalidParametersException, "Provided PixelData has invalid format. Needed: " + toString(mFormat) + ". Got: " + toString(data.format));

		if(mWidth != data.getWidth() || mHeight != data.getHeight() || mDepth != data.getDepth())
		{
			CM_EXCEPT(InvalidParametersException, "Provided PixelData size doesn't match the texture size." \
				" Width: " + toString(mWidth) + "/" + toString(data.getWidth()) + 
				" Height: " + toString(mHeight) + "/" + toString(data.getHeight()) + 
				" Depth: " + toString(mDepth) + "/" + toString(data.getDepth()));
		}

		getBuffer_internal(face, mip)->blitFromMemory(data);
	}

	PixelDataPtr Texture::getRawPixels(UINT32 face, UINT32 mip)
	{
		AsyncOp op = RenderSystem::instancePtr()->queueReturnCommand(boost::bind(&Texture::getRawPixels_internal, this, face, mip, _1), true);

		return op.getReturnValue<PixelDataPtr>();
	}

	AsyncOp Texture::getRawPixels_async(UINT32 face, UINT32 mip)
	{
		return RenderSystem::instancePtr()->queueReturnCommand(boost::bind(&Texture::getRawPixels_internal, this, face, mip, _1));
	}

	void Texture::getRawPixels_internal(UINT32 face, UINT32 mip, AsyncOp& op)
	{
		THROW_IF_NOT_RENDER_THREAD;

		if(mip < 0 || mip > mNumMipmaps)
			CM_EXCEPT(InvalidParametersException, "Invalid mip level: " + toString(mip) + ". Min is 0, max is " + toString(mNumMipmaps));

		if(face < 0 || mip > getNumFaces())
			CM_EXCEPT(InvalidParametersException, "Invalid face index: " + toString(face) + ". Min is 0, max is " + toString(getNumFaces()));

		UINT32 numMips = getNumMipmaps();

		UINT32 totalSize = 0;
		UINT32 width = getWidth();
		UINT32 height = getHeight();
		UINT32 depth = getDepth();

		for(UINT32 j = 0; j <= mip; j++)
		{
			totalSize = PixelUtil::getMemorySize(width, height, depth, mFormat);

			if(width != 1) width /= 2;
			if(height != 1) height /= 2;
			if(depth != 1) depth /= 2;
		}

		UINT8* buffer = new UINT8[totalSize]; 
		PixelDataPtr src(new PixelData(width, height, depth, getFormat(), buffer, true));

		getBuffer_internal(face, mip)->blitToMemory(*src);

		op.completeOperation(src);
	}
	//-----------------------------------------------------------------------------
	void Texture::unloadImpl(void)
	{
		THROW_IF_NOT_RENDER_THREAD;

		freeInternalResources();
	}
    //-----------------------------------------------------------------------------   
    void Texture::copy_internal( TexturePtr& target )
    {
		THROW_IF_NOT_RENDER_THREAD;

        if(target->getNumFaces() != getNumFaces())
        {
            CM_EXCEPT(InvalidParametersException, 
                "Texture types must match");
        }

        size_t numMips = std::min(getNumMipmaps(), target->getNumMipmaps());
        for(unsigned int face=0; face<getNumFaces(); face++)
        {
            for(unsigned int mip=0; mip<=numMips; mip++)
            {
                target->getBuffer_internal(face, mip)->blit(getBuffer_internal(face, mip));
            }
        }
    }
	//----------------------------------------------------------------------------- 
	void Texture::throwIfNotRenderThread() const
	{
		if(CM_THREAD_CURRENT_ID != RenderSystem::instancePtr()->getRenderThreadId())
			CM_EXCEPT(InternalErrorException, "Calling an internal texture method from a non-render thread!");
	}

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/

	RTTITypeBase* Texture::getRTTIStatic()
	{
		return TextureRTTI::instance();
	}

	RTTITypeBase* Texture::getRTTI() const
	{
		return Texture::getRTTIStatic();
	}

	/************************************************************************/
	/* 								STATICS	                      			*/
	/************************************************************************/
	TexturePtr Texture::create(TextureType texType, UINT32 width, UINT32 height, UINT32 depth, 
		int num_mips, PixelFormat format, int usage, bool hwGammaCorrection, UINT32 fsaa, const String& fsaaHint)
	{
		return TextureManager::instance().create(texType, 
			width, height, depth, num_mips, format, usage, hwGammaCorrection, fsaa, fsaaHint);
	}
	
	TexturePtr Texture::create(TextureType texType, UINT32 width, UINT32 height, 
		int num_mips, PixelFormat format, int usage, bool hwGammaCorrection, UINT32 fsaa, const String& fsaaHint)
	{
		return TextureManager::instance().create(texType, 
			width, height, num_mips, format, usage, hwGammaCorrection, fsaa, fsaaHint);
	}
}

#undef THROW_IF_NOT_RENDER_THREAD
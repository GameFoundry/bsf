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
#include "CmPixelBuffer.h"
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
		PixelFormat format, int usage, bool hwGamma, UINT32 fsaa, const String& fsaaHint, bool internalCall)
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

		if(internalCall)
			initialize_internal();
		else
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
	void Texture::getCustomAttribute(const String& name, void* pData)
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
		clearBufferViews();
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
		PixelData myData = lock(GBL_WRITE_ONLY_DISCARD, mip, face);
		memcpy(myData.data, data.data, data.getConsecutiveSize());
		unlock();
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
		PixelDataPtr dst(new PixelData(width, height, depth, getFormat(), buffer, true));

		PixelData myData = lock(GBL_READ_ONLY, mip, face);

#if CM_DEBUG_MODE
		if(dst->getConsecutiveSize() != myData.getConsecutiveSize())
		{
			unlock();
			CM_EXCEPT(InternalErrorException, "Buffer sizes don't match");
		}
#endif

		memcpy(dst->data, myData.data, dst->getConsecutiveSize());
		unlock();

		op.completeOperation(dst);
	}
	//----------------------------------------------------------------------------
	PixelData Texture::lock(GpuLockOptions options, UINT32 mipLevel, UINT32 face)
	{
		THROW_IF_NOT_RENDER_THREAD;

		if(mipLevel < 0 || mipLevel > mNumMipmaps)
			CM_EXCEPT(InvalidParametersException, "Invalid mip level: " + toString(mipLevel) + ". Min is 0, max is " + toString(getNumMipmaps()));

		if(face < 0 || face >= getNumFaces())
			CM_EXCEPT(InvalidParametersException, "Invalid face index: " + toString(face) + ". Min is 0, max is " + toString(getNumFaces()));

		return lockImpl(options, mipLevel, face);
	}
	//-----------------------------------------------------------------------------
	void Texture::unlock()
	{
		unlockImpl();
	}
	//-----------------------------------------------------------------------------
	void Texture::copy(TexturePtr& target)
	{
		if (target->getUsage() != this->getUsage() ||
			target->getTextureType() != this->getTextureType())
		{
			CM_EXCEPT(InvalidParametersException, "Source and destination textures must be of same type and must have the same usage and type.");
		}

		if(getWidth() != target->getWidth() || getHeight() != target->getHeight() || getDepth() != target->getDepth())
		{
			CM_EXCEPT(InvalidParametersException, "Texture sizes don't match." \
				" Width: " + toString(getWidth()) + "/" + toString(target->getWidth()) + 
				" Height: " + toString(getHeight()) + "/" + toString(target->getHeight()) + 
				" Depth: " + toString(getDepth()) + "/" + toString(target->getDepth()));
		}

		if(getNumFaces() != target->getNumFaces())
		{
			CM_EXCEPT(InvalidParametersException, "Number of texture faces doesn't match." \
				" Num faces: " + toString(getNumFaces()) + "/" + toString(target->getNumFaces()));
		}

		if(getNumMipmaps() != target->getNumMipmaps())
		{
			CM_EXCEPT(InvalidParametersException, "Number of mipmaps doesn't match." \
				" Num mipmaps: " + toString(getNumMipmaps()) + "/" + toString(target->getNumMipmaps()));
		}

		copyImpl(target);
	}
	//-----------------------------------------------------------------------------
	void Texture::unloadImpl(void)
	{
		THROW_IF_NOT_RENDER_THREAD;

		freeInternalResources();
	}
	//----------------------------------------------------------------------------- 
	void Texture::throwIfNotRenderThread() const
	{
		if(CM_THREAD_CURRENT_ID != RenderSystem::instancePtr()->getRenderThreadId())
			CM_EXCEPT(InternalErrorException, "Calling an internal texture method from a non-render thread!");
	}

	/************************************************************************/
	/* 								TEXTURE VIEW                      		*/
	/************************************************************************/

	TextureView* Texture::createView(TEXTURE_VIEW_DESC& _desc)
	{
		return new TextureView(this, _desc);
	}

	void Texture::destroyView(TextureView* view)
	{
		if(view != nullptr)
			delete view;
	}

	void Texture::clearBufferViews()
	{
		for(auto iter = mTextureViews.begin(); iter != mTextureViews.end(); ++iter)
		{
			destroyView(iter->second->view);
			delete iter->second;
		}

		mTextureViews.clear();
	}

	TextureView* Texture::requestView(UINT32 mostDetailMip, UINT32 numMips, UINT32 firstArraySlice, UINT32 numArraySlices, GpuViewUsage usage)
	{
		TEXTURE_VIEW_DESC key;
		key.mostDetailMip = mostDetailMip;
		key.numMips = numMips;
		key.firstArraySlice = firstArraySlice;
		key.numArraySlices = numArraySlices;
		key.usage = usage;

		auto iterFind = mTextureViews.find(key);
		if(iterFind == mTextureViews.end())
		{
			TextureView* newView = createView(key);
			mTextureViews[key] = new TextureViewReference(newView);

			iterFind = mTextureViews.find(key);
		}

		iterFind->second->refCount++;
		return iterFind->second->view;
	}

	void Texture::releaseView(TextureView* view)
	{
		TextureView key(*view);

		auto iterFind = mTextureViews.find(view->getDesc());
		if(iterFind == mTextureViews.end())
		{
			CM_EXCEPT(InternalErrorException, "Trying to release a texture view that doesn't exist!");
		}

		iterFind->second->refCount--;

		if(iterFind->second->refCount == 0)
		{
			TextureViewReference* toRemove = iterFind->second;

			mTextureViews.erase(iterFind);

			destroyView(toRemove->view);
			delete toRemove;
		}
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
		return TextureManager::instance().createTexture(texType, 
			width, height, depth, num_mips, format, usage, hwGammaCorrection, fsaa, fsaaHint);
	}
	
	TexturePtr Texture::create(TextureType texType, UINT32 width, UINT32 height, 
		int num_mips, PixelFormat format, int usage, bool hwGammaCorrection, UINT32 fsaa, const String& fsaaHint)
	{
		return TextureManager::instance().createTexture(texType, 
			width, height, num_mips, format, usage, hwGammaCorrection, fsaa, fsaaHint);
	}
}

#undef THROW_IF_NOT_RENDER_THREAD
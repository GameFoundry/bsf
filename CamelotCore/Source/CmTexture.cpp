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
#include "CmCoreThread.h"
#include "CmAsyncOp.h"


namespace CamelotFramework {
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

		Resource::initialize();
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

	void Texture::writeSubresource(UINT32 subresourceIdx, const GpuResourceData& data)
	{
		THROW_IF_NOT_CORE_THREAD;

		if(data.getTypeId() != TID_PixelData)
			CM_EXCEPT(InvalidParametersException, "Invalid GpuResourceData type. Only PixelData is supported.");

		const PixelData& pixelData = static_cast<const PixelData&>(data);

		UINT32 face = 0;
		UINT32 mip = 0;
		mapFromSubresourceIdx(subresourceIdx, face, mip);

		PixelData myData = lock(GBL_WRITE_ONLY_DISCARD, mip, face);
		PixelUtil::bulkPixelConversion(pixelData, myData);
		unlock();
	}

	void Texture::readSubresource(UINT32 subresourceIdx, GpuResourceData& data)
	{
		THROW_IF_NOT_CORE_THREAD;

		if(data.getTypeId() != TID_PixelData)
			CM_EXCEPT(InvalidParametersException, "Invalid GpuResourceData type. Only PixelData is supported.");

		PixelData& pixelData = static_cast<PixelData&>(data);

		UINT32 face = 0;
		UINT32 mip = 0;
		mapFromSubresourceIdx(subresourceIdx, face, mip);

		PixelData myData = lock(GBL_READ_ONLY, mip, face);

#if CM_DEBUG_MODE
		if(pixelData.getConsecutiveSize() != myData.getConsecutiveSize())
		{
			unlock();
			CM_EXCEPT(InternalErrorException, "Buffer sizes don't match");
		}
#endif

		PixelUtil::bulkPixelConversion(myData, pixelData);

		unlock();
	}

	PixelDataPtr Texture::allocateSubresourceBuffer(UINT32 subresourceIdx) const
	{
		UINT32 face = 0;
		UINT32 mip = 0;
		mapFromSubresourceIdx(subresourceIdx, face, mip);

		UINT32 numMips = getNumMipmaps();
		UINT32 width = getWidth();
		UINT32 height = getHeight();
		UINT32 depth = getDepth();

		UINT32 totalSize = PixelUtil::getMemorySize(width, height, depth, mFormat);

		for(UINT32 j = 0; j < mip; j++)
		{
			totalSize = PixelUtil::getMemorySize(width, height, depth, mFormat);

			if(width != 1) width /= 2;
			if(height != 1) height /= 2;
			if(depth != 1) depth /= 2;
		}

		PixelDataPtr dst = cm_shared_ptr<PixelData, PoolAlloc>(width, height, depth, getFormat());

		dst->allocateInternalBuffer();

		return dst;
	}

	void Texture::mapFromSubresourceIdx(UINT32 subresourceIdx, UINT32& face, UINT32& mip) const
	{
		UINT32 numMipmaps = getNumMipmaps() + 1;

		face = Math::FloorToInt((subresourceIdx) / (float)numMipmaps);
		mip = subresourceIdx % numMipmaps;
	}

	UINT32 Texture::mapToSubresourceIdx(UINT32 face, UINT32 mip) const
	{
		return face * (getNumMipmaps() + 1) + mip;
	}

	//----------------------------------------------------------------------------
	PixelData Texture::lock(GpuLockOptions options, UINT32 mipLevel, UINT32 face)
	{
		THROW_IF_NOT_CORE_THREAD;

		if(mipLevel < 0 || mipLevel > mNumMipmaps)
			CM_EXCEPT(InvalidParametersException, "Invalid mip level: " + toString(mipLevel) + ". Min is 0, max is " + toString(getNumMipmaps()));

		if(face < 0 || face >= getNumFaces())
			CM_EXCEPT(InvalidParametersException, "Invalid face index: " + toString(face) + ". Min is 0, max is " + toString(getNumFaces()));

		return lockImpl(options, mipLevel, face);
	}
	//-----------------------------------------------------------------------------
	void Texture::unlock()
	{
		THROW_IF_NOT_CORE_THREAD;

		unlockImpl();
	}
	//-----------------------------------------------------------------------------
	void Texture::copy(TexturePtr& target)
	{
		THROW_IF_NOT_CORE_THREAD;

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

	/************************************************************************/
	/* 								TEXTURE VIEW                      		*/
	/************************************************************************/

	TextureViewPtr Texture::createView()
	{
		TextureViewPtr viewPtr = cm_core_ptr<TextureView, PoolAlloc>(new (cm_alloc<TextureView, PoolAlloc>()) TextureView());
		viewPtr->setThisPtr(viewPtr);

		return viewPtr;
	}

	void Texture::clearBufferViews()
	{
		mTextureViews.clear();
	}

	TextureViewPtr Texture::requestView(TexturePtr texture, UINT32 mostDetailMip, UINT32 numMips, UINT32 firstArraySlice, UINT32 numArraySlices, GpuViewUsage usage)
	{
		assert(texture != nullptr);

		TEXTURE_VIEW_DESC key;
		key.mostDetailMip = mostDetailMip;
		key.numMips = numMips;
		key.firstArraySlice = firstArraySlice;
		key.numArraySlices = numArraySlices;
		key.usage = usage;

		auto iterFind = texture->mTextureViews.find(key);
		if(iterFind == texture->mTextureViews.end())
		{
			TextureViewPtr newView = texture->createView();
			newView->initialize(texture, key);
			texture->mTextureViews[key] = new (cm_alloc<TextureViewReference, PoolAlloc>()) TextureViewReference(newView);

			iterFind = texture->mTextureViews.find(key);
		}

		iterFind->second->refCount++;
		return iterFind->second->view;
	}

	void Texture::releaseView(TextureViewPtr view)
	{
		assert(view != nullptr);

		TexturePtr texture = view->getTexture();

		auto iterFind = texture->mTextureViews.find(view->getDesc());
		if(iterFind == texture->mTextureViews.end())
		{
			CM_EXCEPT(InternalErrorException, "Trying to release a texture view that doesn't exist!");
		}

		iterFind->second->refCount--;

		if(iterFind->second->refCount == 0)
		{
			TextureViewReference* toRemove = iterFind->second;

			texture->mTextureViews.erase(iterFind);

			cm_delete<PoolAlloc>(toRemove);
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
	HTexture Texture::create(TextureType texType, UINT32 width, UINT32 height, UINT32 depth, 
		int num_mips, PixelFormat format, int usage, bool hwGammaCorrection, UINT32 fsaa, const String& fsaaHint)
	{
		TexturePtr texturePtr = TextureManager::instance().createTexture(texType, 
			width, height, depth, num_mips, format, usage, hwGammaCorrection, fsaa, fsaaHint);

		return static_resource_cast<Texture>(Resource::_createResourceHandle(texturePtr));
	}
	
	HTexture Texture::create(TextureType texType, UINT32 width, UINT32 height, 
		int num_mips, PixelFormat format, int usage, bool hwGammaCorrection, UINT32 fsaa, const String& fsaaHint)
	{
		TexturePtr texturePtr = TextureManager::instance().createTexture(texType, 
			width, height, num_mips, format, usage, hwGammaCorrection, fsaa, fsaaHint);

		return static_resource_cast<Texture>(Resource::_createResourceHandle(texturePtr));
	}

	HTexture Texture::dummy()
	{
		return TextureManager::instance().getDummyTexture();
	}
}
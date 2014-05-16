#include "CmPixelBuffer.h"
#include "CmTexture.h"
#include "CmTextureRTTI.h"
#include "CmDataStream.h"
#include "CmException.h"
#include "CmDebug.h"
#include "CmCoreThread.h"
#include "CmAsyncOp.h"
#include "CmResources.h"

namespace BansheeEngine 
{
    Texture::Texture()
        : 
            mHeight(32),
            mWidth(32),
            mDepth(1),
            mNumMipmaps(0),
			mHwGamma(false),
			mFSAA(0),
            mTextureType(TEX_TYPE_2D),            
            mFormat(PF_UNKNOWN),
            mUsage(TU_DEFAULT)
    {
        
    }

	void Texture::initialize(TextureType textureType, UINT32 width, UINT32 height, UINT32 depth, UINT32 numMipmaps, 
		PixelFormat format, int usage, bool hwGamma, UINT32 fsaa, const String& fsaaHint)
	{
		mTextureType = textureType;
		mWidth = width;
		mHeight = height;
		mDepth = depth;
		mNumMipmaps = numMipmaps;
		mUsage = usage;
		mHwGamma = hwGamma;
		mFSAA = fsaa;
		mFSAAHint = fsaaHint;

		// Adjust format if required
		mFormat = TextureManager::instance().getNativeFormat(mTextureType, format, mUsage);
		mSize = getNumFaces() * PixelUtil::getMemorySize(mWidth, mHeight, mDepth, mFormat);

		Resource::initialize();
	}

    bool Texture::hasAlpha() const
    {
        return PixelUtil::hasAlpha(mFormat);
    }

	UINT32 Texture::calculateSize() const
	{
        return getNumFaces() * PixelUtil::getMemorySize(mWidth, mHeight, mDepth, mFormat);
	}

	UINT32 Texture::getNumFaces(void) const
	{
		return getTextureType() == TEX_TYPE_CUBE_MAP ? 6 : 1;
	}

	void Texture::writeSubresource(UINT32 subresourceIdx, const GpuResourceData& data, bool discardEntireBuffer)
	{
		THROW_IF_NOT_CORE_THREAD;

		if(data.getTypeId() != TID_PixelData)
			CM_EXCEPT(InvalidParametersException, "Invalid GpuResourceData type. Only PixelData is supported.");

		if(discardEntireBuffer)
		{
			if(mUsage != TU_DYNAMIC)
			{
				LOGWRN("Buffer discard is enabled but buffer was not created as dynamic. Disabling discard.");
				discardEntireBuffer = false;
			}
		}
		else
		{
			if(mUsage == TU_DYNAMIC)
			{
				LOGWRN("Buffer discard is not enabled but buffer was not created as dynamic. Enabling discard.");
				discardEntireBuffer = true;
			}
		}

		const PixelData& pixelData = static_cast<const PixelData&>(data);

		if(pixelData.getWidth() != getWidth() ||pixelData.getHeight() != getHeight() || 
			pixelData.getDepth() != getDepth() || pixelData.getFormat() != getFormat())
		{
			CM_EXCEPT(RenderingAPIException, "Provided buffer is not of valid dimensions or format in order to write to this texture.");
		}

		UINT32 face = 0;
		UINT32 mip = 0;
		mapFromSubresourceIdx(subresourceIdx, face, mip);

		writeData(pixelData, mip, face, discardEntireBuffer);
	}

	void Texture::readSubresource(UINT32 subresourceIdx, GpuResourceData& data)
	{
		THROW_IF_NOT_CORE_THREAD;

		if(data.getTypeId() != TID_PixelData)
			CM_EXCEPT(InvalidParametersException, "Invalid GpuResourceData type. Only PixelData is supported.");

		PixelData& pixelData = static_cast<PixelData&>(data);

		if(pixelData.getWidth() != getWidth() ||pixelData.getHeight() != getHeight() || 
			pixelData.getDepth() != getDepth() || pixelData.getFormat() != getFormat())
		{
			CM_EXCEPT(RenderingAPIException, "Provided buffer is not of valid dimensions or format in order to read from this texture.");
		}

		UINT32 face = 0;
		UINT32 mip = 0;
		mapFromSubresourceIdx(subresourceIdx, face, mip);

		readData(pixelData, mip, face);
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

		face = Math::floorToInt((subresourceIdx) / (float)numMipmaps);
		mip = subresourceIdx % numMipmaps;
	}

	UINT32 Texture::mapToSubresourceIdx(UINT32 face, UINT32 mip) const
	{
		return face * (getNumMipmaps() + 1) + mip;
	}

	PixelData Texture::lock(GpuLockOptions options, UINT32 mipLevel, UINT32 face)
	{
		THROW_IF_NOT_CORE_THREAD;

		if(mipLevel < 0 || mipLevel > mNumMipmaps)
			CM_EXCEPT(InvalidParametersException, "Invalid mip level: " + toString(mipLevel) + ". Min is 0, max is " + toString(getNumMipmaps()));

		if(face < 0 || face >= getNumFaces())
			CM_EXCEPT(InvalidParametersException, "Invalid face index: " + toString(face) + ". Min is 0, max is " + toString(getNumFaces()));

		return lockImpl(options, mipLevel, face);
	}

	void Texture::unlock()
	{
		THROW_IF_NOT_CORE_THREAD;

		unlockImpl();
	}

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
		viewPtr->_setThisPtr(viewPtr);

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
		TexturePtr texturePtr = _createPtr(texType, 
			width, height, depth, num_mips, format, usage, hwGammaCorrection, fsaa, fsaaHint);

		return static_resource_cast<Texture>(gResources().createResourceHandle(texturePtr));
	}
	
	HTexture Texture::create(TextureType texType, UINT32 width, UINT32 height, 
		int num_mips, PixelFormat format, int usage, bool hwGammaCorrection, UINT32 fsaa, const String& fsaaHint)
	{
		TexturePtr texturePtr = _createPtr(texType, 
			width, height, num_mips, format, usage, hwGammaCorrection, fsaa, fsaaHint);

		return static_resource_cast<Texture>(gResources().createResourceHandle(texturePtr));
	}

	TexturePtr Texture::_createPtr(TextureType texType, UINT32 width, UINT32 height, UINT32 depth, 
		int num_mips, PixelFormat format, int usage, bool hwGammaCorrection, UINT32 fsaa, const String& fsaaHint)
	{
		return TextureManager::instance().createTexture(texType, 
			width, height, depth, num_mips, format, usage, hwGammaCorrection, fsaa, fsaaHint);
	}

	TexturePtr Texture::_createPtr(TextureType texType, UINT32 width, UINT32 height, 
		int num_mips, PixelFormat format, int usage, bool hwGammaCorrection, UINT32 fsaa, const String& fsaaHint)
	{
		return TextureManager::instance().createTexture(texType, 
			width, height, num_mips, format, usage, hwGammaCorrection, fsaa, fsaaHint);
	}

	const HTexture& Texture::dummy()
	{
		return TextureManager::instance().getDummyTexture();
	}
}
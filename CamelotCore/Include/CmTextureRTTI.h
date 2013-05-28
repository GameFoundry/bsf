#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmTexture.h"
#include "CmManagedDataBlock.h"
#include "CmMath.h"
#include "CmApplication.h"
#include "CmDeferredRenderContext.h"
#include <boost/preprocessor/comma.hpp>

// DEBUG ONLY
#include "CmTextureManager.h"

namespace CamelotFramework
{
	class CM_EXPORT TextureRTTI : public RTTIType<Texture, Resource, TextureRTTI>
	{
	private:
		CM_SETGET_MEMBER(mSize, UINT32, Texture)
		CM_SETGET_MEMBER(mUUID, String, Texture);
		CM_SETGET_MEMBER(mHeight, UINT32, Texture)
		CM_SETGET_MEMBER(mWidth, UINT32, Texture)
		CM_SETGET_MEMBER(mDepth, UINT32, Texture)
		CM_SETGET_MEMBER(mNumMipmaps, UINT32, Texture)
		CM_SETGET_MEMBER(mHwGamma, bool, Texture)
		CM_SETGET_MEMBER(mFSAA, UINT32, Texture)
		CM_SETGET_MEMBER(mFSAAHint, String, Texture)
		CM_SETGET_MEMBER(mTextureType, TextureType, Texture)
		CM_SETGET_MEMBER(mFormat, PixelFormat, Texture)
		CM_SETGET_MEMBER(mUsage, INT32, Texture)

		PixelDataPtr getPixelData(Texture* obj, UINT32 idx)
		{
			UINT32 face = (size_t)Math::Floor(idx / (float)(obj->getNumMipmaps() + 1));
			UINT32 mipmap = idx % (obj->getNumMipmaps() + 1);

			UINT32 subresourceIdx = obj->mapToSubresourceIdx(face, mipmap);
			PixelDataPtr pixelData = obj->allocateSubresourceBuffer(subresourceIdx);

			GpuResourcePtr sharedTexPtr = std::static_pointer_cast<GpuResource>(obj->getThisPtr());

			gMainSyncedRC().readSubresource(sharedTexPtr, subresourceIdx, *pixelData);
			gMainSyncedRC().submitToGpu(true); // We need the data right away, so execute the context and wait until we get it

			return pixelData;
		}

		void setPixelData(Texture* obj, UINT32 idx, PixelDataPtr data)
		{
			vector<PixelDataPtr>::type* pixelData = boost::any_cast<vector<PixelDataPtr>::type*>(obj->mRTTIData);

			(*pixelData)[idx] = data;
		}

		UINT32 getPixelDataArraySize(Texture* obj)
		{
			return obj->getNumFaces() * (obj->getNumMipmaps() + 1);
		}

		void setPixelDataArraySize(Texture* obj, UINT32 size)
		{
			vector<PixelDataPtr>::type* pixelData = boost::any_cast<vector<PixelDataPtr>::type*>(obj->mRTTIData);

			pixelData->resize(size);
		}

	public:
		TextureRTTI()
		{
			CM_ADD_PLAINFIELD(mSize, 0, TextureRTTI)
			CM_ADD_PLAINFIELD(mUUID, 1, TextureRTTI)
			CM_ADD_PLAINFIELD(mHeight, 2, TextureRTTI)
			CM_ADD_PLAINFIELD(mWidth, 3, TextureRTTI)
			CM_ADD_PLAINFIELD(mDepth, 4, TextureRTTI)
			CM_ADD_PLAINFIELD(mNumMipmaps, 5, TextureRTTI)
			CM_ADD_PLAINFIELD(mHwGamma, 6, TextureRTTI)
			CM_ADD_PLAINFIELD(mFSAA, 7, TextureRTTI)
			CM_ADD_PLAINFIELD(mFSAAHint, 8, TextureRTTI)
			CM_ADD_PLAINFIELD(mTextureType, 9, TextureRTTI)
			CM_ADD_PLAINFIELD(mFormat, 10, TextureRTTI)
			CM_ADD_PLAINFIELD(mUsage, 11, TextureRTTI)

			addReflectablePtrArrayField("mPixelData", 12, &TextureRTTI::getPixelData, &TextureRTTI::getPixelDataArraySize, 
				&TextureRTTI::setPixelData, &TextureRTTI::setPixelDataArraySize);
		}

		virtual void onDeserializationStarted(IReflectable* obj)
		{
			Texture* texture = static_cast<Texture*>(obj);

			texture->mRTTIData = CM_NEW(vector<PixelDataPtr>::type, PoolAlloc) vector<PixelDataPtr>::type();
		}

		virtual void onDeserializationEnded(IReflectable* obj)
		{
			Texture* texture = static_cast<Texture*>(obj);

			if(texture->mRTTIData.empty())
				return;

			// A bit clumsy initializing with already set values, but I feel its better than complicating things and storing the values
			// in mRTTIData.
			texture->initialize(texture->getTextureType(), texture->getWidth(), texture->getHeight(), texture->getDepth(), 
				texture->getNumMipmaps(), texture->getFormat(), texture->getUsage(), texture->isHardwareGammaEnabled(), 
				texture->getFSAA(), texture->getFSAAHint());

			vector<PixelDataPtr>::type* pixelData = boost::any_cast<vector<PixelDataPtr>::type*>(texture->mRTTIData);
			for(size_t i = 0; i < pixelData->size(); i++)
			{
				UINT32 face = (size_t)Math::Floor(i / (float)(texture->getNumMipmaps() + 1));
				UINT32 mipmap = i % (texture->getNumMipmaps() + 1);

				UINT32 subresourceIdx = texture->mapToSubresourceIdx(face, mipmap);

				GpuResourcePtr sharedTexPtr = std::static_pointer_cast<GpuResource>(texture->getThisPtr());
				gMainSyncedRC().writeSubresource(sharedTexPtr, subresourceIdx, *pixelData->at(i));
			}

			gMainSyncedRC().submitToGpu(true); // TODO - Possibly we can avoid this. I don't see a reason we need to wait for the update to complete.

			CM_DELETE(pixelData, vector<PixelDataPtr BOOST_PP_COMMA() StdGenAlloc<PixelDataPtr>>, PoolAlloc);
			texture->mRTTIData = nullptr;	
		}

		virtual const String& getRTTIName()
		{
			static String name = "Texture";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_Texture;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return TextureManager::instance().createEmpty();
		}
	};
}
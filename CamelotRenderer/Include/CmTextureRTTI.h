#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmTexture.h"
#include "CmManagedDataBlock.h"
#include "CmMath.h"

// DEBUG ONLY
#include "CmTextureManager.h"

namespace CamelotEngine
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
		CM_SETGET_MEMBER(mTextureType, TextureType, Texture)
		CM_SETGET_MEMBER(mFormat, PixelFormat, Texture)
		CM_SETGET_MEMBER(mUsage, INT32, Texture)

		PixelDataPtr getPixelData(Texture* obj, UINT32 idx)
		{
			UINT32 face = (size_t)Math::Floor(idx / (float)(obj->getNumMipmaps() + 1));
			UINT32 mipmap = idx % (obj->getNumMipmaps() + 1);

			return obj->getRawPixels(face, mipmap);
		}

		void setPixelData(Texture* obj, UINT32 idx, PixelDataPtr data)
		{
			vector<PixelDataPtr>::type* pixelData = boost::any_cast<vector<PixelDataPtr>::type*>(obj->mRTTIData);

			(*pixelData)[idx] = data;
		}

		UINT32 getPixelDataArraySize(Texture* obj)
		{
			return obj->getNumFaces() * obj->getNumMipmaps();
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
			CM_ADD_PLAINFIELD(mTextureType, 7, TextureRTTI)
			CM_ADD_PLAINFIELD(mFormat, 8, TextureRTTI)
			CM_ADD_PLAINFIELD(mUsage, 9, TextureRTTI)

			addReflectablePtrArrayField("mPixelData", 12, &TextureRTTI::getPixelData, &TextureRTTI::getPixelDataArraySize, 
				&TextureRTTI::setPixelData, &TextureRTTI::setPixelDataArraySize);
		}

		virtual void onDeserializationStarted(IReflectable* obj)
		{
			Texture* texture = static_cast<Texture*>(obj);

			texture->mRTTIData = new vector<PixelDataPtr>::type();
		}

		virtual void onDeserializationEnded(IReflectable* obj)
		{
			Texture* texture = static_cast<Texture*>(obj);

			if(texture->mRTTIData.empty())
				return;

			// A bit clumsy initializing with already set values, but I feel its better than complicating things and storing the values
			// in mRTTIData.
			texture->initialize(texture->getTextureType(), texture->getWidth(), texture->getHeight(), texture->getDepth(), 
				texture->getNumMipmaps(), texture->getFormat(), texture->getUsage(), texture->isHardwareGammaEnabled());

			vector<PixelDataPtr>::type* pixelData = boost::any_cast<vector<PixelDataPtr>::type*>(texture->mRTTIData);
			for(size_t i = 0; i < pixelData->size(); i++)
			{
				UINT32 face = (size_t)Math::Floor(i / (float)(texture->getNumMipmaps() + 1));
				UINT32 mipmap = i % (texture->getNumMipmaps() + 1);

				texture->setRawPixels_async(*pixelData->at(i), face, mipmap);
			}

			delete pixelData;
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
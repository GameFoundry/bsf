#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmTexture.h"
#include "CmTextureData.h"
#include "CmManagedDataBlock.h"

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
		CM_SETGET_MEMBER(mNumRequestedMipmaps, UINT32, Texture)
		CM_SETGET_MEMBER(mNumMipmaps, UINT32, Texture)
		CM_SETGET_MEMBER(mMipmapsHardwareGenerated, bool, Texture)
		CM_SETGET_MEMBER(mGamma, float, Texture)
		CM_SETGET_MEMBER(mHwGamma, bool, Texture)
		CM_SETGET_MEMBER(mFSAA, UINT32, Texture)
		CM_SETGET_MEMBER(mFSAAHint, String, Texture)
		CM_SETGET_MEMBER(mTextureType, TextureType, Texture)
		CM_SETGET_MEMBER(mFormat, PixelFormat, Texture)
		CM_SETGET_MEMBER(mUsage, INT32, Texture)
		CM_SETGET_MEMBER(mDesiredFormat, PixelFormat, Texture)
		CM_SETGET_MEMBER(mDesiredIntegerBitDepth, UINT16, Texture)
		CM_SETGET_MEMBER(mDesiredFloatBitDepth, UINT16, Texture)

		std::shared_ptr<TextureData> getTextureData(Texture* obj, UINT32 face)
		{
			return obj->getTextureData(face);
		}

		void setTextureData(Texture* obj, UINT32 face, TextureDataPtr data)
		{
			return obj->setTextureData(face, data);
		}

		UINT32 getTextureDataArraySize(Texture* obj)
		{
			return obj->getNumFaces();
		}

		void setTextureDataArraySize(Texture* obj, UINT32 size)
		{
			// Not allowed to change size this way
		}

	public:
		TextureRTTI()
		{
			CM_ADD_PLAINFIELD(mSize, 0, TextureRTTI)
			CM_ADD_PLAINFIELD(mUUID, 1, TextureRTTI)
			CM_ADD_PLAINFIELD(mHeight, 2, TextureRTTI)
			CM_ADD_PLAINFIELD(mWidth, 3, TextureRTTI)
			CM_ADD_PLAINFIELD(mDepth, 4, TextureRTTI)
			CM_ADD_PLAINFIELD(mNumRequestedMipmaps, 5, TextureRTTI)
			CM_ADD_PLAINFIELD(mNumMipmaps, 6, TextureRTTI)
			CM_ADD_PLAINFIELD(mMipmapsHardwareGenerated, 7, TextureRTTI)
			CM_ADD_PLAINFIELD(mGamma, 8, TextureRTTI)
			CM_ADD_PLAINFIELD(mHwGamma, 9, TextureRTTI)
			CM_ADD_PLAINFIELD(mFSAA, 10, TextureRTTI)
			CM_ADD_PLAINFIELD(mFSAAHint, 11, TextureRTTI)
			CM_ADD_PLAINFIELD(mTextureType, 12, TextureRTTI)
			CM_ADD_PLAINFIELD(mFormat, 13, TextureRTTI)
			CM_ADD_PLAINFIELD(mUsage, 14, TextureRTTI)
			CM_ADD_PLAINFIELD(mDesiredFormat, 15, TextureRTTI)
			CM_ADD_PLAINFIELD(mDesiredIntegerBitDepth, 16, TextureRTTI)
			CM_ADD_PLAINFIELD(mDesiredFloatBitDepth, 17, TextureRTTI)

			addReflectablePtrArrayField("mTextureData", 18, &TextureRTTI::getTextureData, &TextureRTTI::getTextureDataArraySize, 
				&TextureRTTI::setTextureData, &TextureRTTI::setTextureDataArraySize);
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
			// DEBUG ONLY - Remove this after I implement RTTI types for specific texture types
			return TextureManager::instance().create(TEX_TYPE_2D, 128, 128, 1, PF_A8B8G8R8);

			//CM_EXCEPT(InternalErrorException, "Cannot instantiate abstract class!");
		}
	};
}
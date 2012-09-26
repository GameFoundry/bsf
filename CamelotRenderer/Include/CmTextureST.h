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
	class CM_EXPORT TextureST : public RTTIType<Texture, Resource, TextureST>
	{
	private:
		CM_SETGET_MEMBER(mSize, UINT32, Texture)
		CM_SETGET_MEMBER(mSourceUUID, UUID, Texture);
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
		TextureST()
		{
			CM_ADD_PLAINFIELD(mSize, 0, TextureST)
			CM_ADD_PLAINFIELD(mSourceUUID, 1, TextureST)
			CM_ADD_PLAINFIELD(mHeight, 2, TextureST)
			CM_ADD_PLAINFIELD(mWidth, 3, TextureST)
			CM_ADD_PLAINFIELD(mDepth, 4, TextureST)
			CM_ADD_PLAINFIELD(mNumRequestedMipmaps, 5, TextureST)
			CM_ADD_PLAINFIELD(mNumMipmaps, 6, TextureST)
			CM_ADD_PLAINFIELD(mMipmapsHardwareGenerated, 7, TextureST)
			CM_ADD_PLAINFIELD(mGamma, 8, TextureST)
			CM_ADD_PLAINFIELD(mHwGamma, 9, TextureST)
			CM_ADD_PLAINFIELD(mFSAA, 10, TextureST)
			CM_ADD_PLAINFIELD(mFSAAHint, 11, TextureST)
			CM_ADD_PLAINFIELD(mTextureType, 12, TextureST)
			CM_ADD_PLAINFIELD(mFormat, 13, TextureST)
			CM_ADD_PLAINFIELD(mUsage, 14, TextureST)
			CM_ADD_PLAINFIELD(mDesiredFormat, 15, TextureST)
			CM_ADD_PLAINFIELD(mDesiredIntegerBitDepth, 16, TextureST)
			CM_ADD_PLAINFIELD(mDesiredFloatBitDepth, 17, TextureST)

			addReflectablePtrArrayField("mTextureData", 18, &TextureST::getTextureData, &TextureST::getTextureDataArraySize, 
				&TextureST::setTextureData, &TextureST::setTextureDataArraySize);
		}

		virtual const String& getRTTIName()
		{
			static String name = "Texture";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return 101;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			// DEBUG ONLY - Remove this after I implement RTTI types for specific texture types
			return TextureManager::instance().create(TEX_TYPE_2D, 128, 128, 1, PF_A8B8G8R8);

			//CM_EXCEPT(InternalErrorException, "Cannot instantiate abstract class!");
		}
	};
}
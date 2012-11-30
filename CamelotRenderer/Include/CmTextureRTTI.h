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
		CM_SETGET_MEMBER(mNumMipmaps, UINT32, Texture)
		CM_SETGET_MEMBER(mHwGamma, bool, Texture)
		CM_SETGET_MEMBER(mFSAA, UINT32, Texture)
		CM_SETGET_MEMBER(mFSAAHint, String, Texture)
		CM_SETGET_MEMBER(mTextureType, TextureType, Texture)
		CM_SETGET_MEMBER(mFormat, PixelFormat, Texture)
		CM_SETGET_MEMBER(mUsage, INT32, Texture)

		std::shared_ptr<TextureData> getTextureData(Texture* obj, UINT32 face)
		{
			return obj->getTextureData(face);
		}

		void setTextureData(Texture* obj, UINT32 face, TextureDataPtr data)
		{
			obj->setTextureData(face, data);
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
			CM_ADD_PLAINFIELD(mNumMipmaps, 5, TextureRTTI)
			CM_ADD_PLAINFIELD(mHwGamma, 6, TextureRTTI)
			CM_ADD_PLAINFIELD(mFSAA, 7, TextureRTTI)
			CM_ADD_PLAINFIELD(mFSAAHint, 8, TextureRTTI)
			CM_ADD_PLAINFIELD(mTextureType, 9, TextureRTTI)
			CM_ADD_PLAINFIELD(mFormat, 10, TextureRTTI)
			CM_ADD_PLAINFIELD(mUsage, 11, TextureRTTI)

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
			return Texture::create(TEX_TYPE_2D, 128, 128, 1, PF_A8B8G8R8);

			//CM_EXCEPT(InternalErrorException, "Cannot instantiate abstract class!");
		}
	};
}
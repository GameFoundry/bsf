#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmTexture.h"
#include "CmTextureData.h"
#include "CmManagedDataBlock.h"

namespace CamelotEngine
{
	class CM_EXPORT TextureST : public RTTIType
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

		//ManagedDataBlock getFaceData(Texture* obj)
		//{
		//	UINT32 totalSize = 0;
		//	vector<TextureDataPtr>::type faces = obj->getTextureData();

		//	for(auto iter = faces.begin(); iter != faces.end(); ++iter)
		//	{
		//		totalSize += (*iter)->getSize();
		//	}

		//	UINT8* faceData = new UINT8[totalSize]; // TODO - Any way to avoid this reallocation? getTextureData allocates the same amount of memory and then
		//											// we allocate it again, copy from buffers and throw away getTextureData memory
		//											//   - TextureData should store everything in one big buffer?
		//											//   - OR make TextureData ISerializable?!

		//	//ManagedDataBlock dataBlock()

		//	return obj->getTextureData(face).get();
		//}

		//void setFaceData(Texture* obj, UINT32 face, TextureData* data)
		//{
		//	return obj->getTextureData(face).get();
		//}

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
	};
}
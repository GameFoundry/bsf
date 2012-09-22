#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmTextureData.h"
#include "CmSerializableType.h"
#include "CmManagedDataBlock.h"

namespace CamelotEngine
{
	class TextureDataST : public SerializableType
	{
		CM_SETGET_MEMBER(mNumMipmaps, UINT32, TextureData)
		CM_SETGET_MEMBER(mWidth, UINT32, TextureData)
		CM_SETGET_MEMBER(mHeight, UINT32, TextureData)
		CM_SETGET_MEMBER(mSize, UINT32, TextureData)
		CM_SETGET_MEMBER(mDepth, UINT32, TextureData)
		CM_SETGET_MEMBER(mFlags, INT32, TextureData)
		CM_SETGET_MEMBER(mBPP, UINT8, TextureData)
		CM_SETGET_MEMBER(mFormat, PixelFormat, TextureData)
		
		ManagedDataBlock getData(TextureData* obj) 
		{
			return ManagedDataBlock(obj->mData, obj->mSize, false);
		}

		void setData(TextureData* obj, ManagedDataBlock val) 
		{ 
			obj->mData = val.getData();
			obj->mSize = val.getSize();
		} 

	public:
		TextureDataST()
		{
			CM_ADD_PLAINFIELD(mNumMipmaps, 0, TextureDataST);
			CM_ADD_PLAINFIELD(mWidth, 1, TextureDataST);
			CM_ADD_PLAINFIELD(mHeight, 2, TextureDataST);
			CM_ADD_PLAINFIELD(mSize, 3, TextureDataST);
			CM_ADD_PLAINFIELD(mDepth, 4, TextureDataST);
			CM_ADD_PLAINFIELD(mFlags, 5, TextureDataST);
			CM_ADD_PLAINFIELD(mBPP, 6, TextureDataST);
			CM_ADD_PLAINFIELD(mFormat, 7, TextureDataST);

			addDataBlockField("Data", 8, &TextureDataST::getData, &TextureDataST::setData);
		}
	};
}
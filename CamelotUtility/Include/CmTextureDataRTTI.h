#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmTextureData.h"
#include "CmRTTIType.h"
#include "CmManagedDataBlock.h"

namespace CamelotEngine
{
	class CM_UTILITY_EXPORT TextureDataRTTI : public RTTIType<TextureData, IReflectable, TextureDataRTTI>
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
		TextureDataRTTI()
		{
			CM_ADD_PLAINFIELD(mNumMipmaps, 0, TextureDataRTTI);
			CM_ADD_PLAINFIELD(mWidth, 1, TextureDataRTTI);
			CM_ADD_PLAINFIELD(mHeight, 2, TextureDataRTTI);
			CM_ADD_PLAINFIELD(mSize, 3, TextureDataRTTI);
			CM_ADD_PLAINFIELD(mDepth, 4, TextureDataRTTI);
			CM_ADD_PLAINFIELD(mFlags, 5, TextureDataRTTI);
			CM_ADD_PLAINFIELD(mBPP, 6, TextureDataRTTI);
			CM_ADD_PLAINFIELD(mFormat, 7, TextureDataRTTI);

			addDataBlockField("Data", 8, &TextureDataRTTI::getData, &TextureDataRTTI::setData);
		}

		virtual const String& getRTTIName()
		{
			static String name = "TextureData";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return 102;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return std::shared_ptr<TextureData>(new TextureData());
		}
	};
}
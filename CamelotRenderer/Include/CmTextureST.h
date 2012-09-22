#pragma once

#include "CmPrerequisites.h"
#include "CmSerializableType.h"
#include "CmTexture.h"

namespace CamelotEngine
{
	class CM_EXPORT TextureST : public SerializableType
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
		}
	};
}
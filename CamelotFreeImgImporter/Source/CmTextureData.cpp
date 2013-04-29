#include "CmTextureData.h"
#include "CmException.h"

namespace CamelotFramework
{
	TextureData::TextureData(UINT32 width, UINT32 height, UINT32 size, 
		PixelFormat format, UINT32 depth, INT32 flags, UINT32 numMipmaps)
		:mWidth(width), mHeight(height), mDepth(depth), mSize(size), mFormat(format),
		mFlags(flags), mNumMipmaps(numMipmaps)
	{
		mBPP = static_cast<UINT8>(PixelUtil::getNumElemBytes(mFormat)) * 8;
		mData = CM_NEW_BYTES(size, ScratchAlloc);
	}

	TextureData::~TextureData()
	{
		if(mData != nullptr)
			CM_DELETE_BYTES(mData, ScratchAlloc);
	}

	PixelData TextureData::getPixels(UINT32 mip)
	{
		if(mip < 0 || mip > mNumMipmaps)
		{
			CM_EXCEPT(InvalidParametersException, "Mip out of range: " + toString(mip) + ". While maximum available mip is: " + toString((mNumMipmaps)));
		}

		// Calculate mipmap offset and size
		UINT8 *offset = const_cast<UINT8*>(getData());
		UINT8 offsetSoFar = 0;

		UINT32 width = getWidth(), height = getHeight(), depth = getDepth();
		UINT32 numMips = getNumMipmaps();

		// Figure out the offsets 
		UINT32 finalWidth = 0, finalHeight = 0, finalDepth = 0;
		for(UINT32 curMip = 0; curMip <= numMips; ++curMip)
		{
			if (curMip == mip)
			{
				offset += offsetSoFar;
				finalWidth = width;
				finalHeight = height;
				finalDepth = depth;
			}
			offsetSoFar += PixelUtil::getMemorySize(width, height, depth, getFormat());

			/// Half size in each dimension
			if(width!=1) width /= 2;
			if(height!=1) height /= 2;
			if(depth!=1) depth /= 2;
		}

		// Return subface as pixelbox
		PixelData src(finalWidth, finalHeight, finalDepth, getFormat());
		src.setExternalBuffer(offset);
		return src;
	}
}
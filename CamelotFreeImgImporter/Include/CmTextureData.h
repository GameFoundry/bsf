#pragma once

#include "CmFreeImgPrerequisites.h"
#include "CmPixelUtil.h"

namespace CamelotEngine
{
	enum TextureDataFlags
	{
		TDF_COMPRESSED = 0x00000001,
		TDF_CUBEMAP    = 0x00000002,
		TDF_3D_TEXTURE = 0x00000004
	};

	class CM_FREEIMG_EXPORT TextureData
	{
	public:
		TextureData(UINT32 width, UINT32 height, UINT32 size, 
			PixelFormat format, UINT8* data, UINT32 depth = 1, INT32 flags = 0, UINT32 numMipmaps = 1);
		~TextureData();

		/** Returns a pointer to the internal image buffer.
		@remarks
			Be careful with this method. You will almost certainly
			prefer to use getPixels, especially with complex images
			which include custom mipmaps.
        */
        UINT8* getData(void) { return mData; }

        /** Returns a const pointer to the internal image buffer.
		@remarks
			Be careful with this method. You will almost certainly
			prefer to use getPixels, especially with complex images
			which include custom mipmaps.
        */
        const UINT8* getData() const { return mData; } 

        /** Returns the size of the data buffer.
        */
		UINT32 getSize() const { return mSize; }

        /** Returns the number of mipmaps contained in the image.
        */
		UINT32 getNumMipmaps() const { return mNumMipmaps; }

        /** Returns true if the image has the appropriate flag set.
        */
		bool hasFlag(const TextureDataFlags flag) const { return (mFlags & flag) != 0; }

        /** Gets the width of the image in pixels.
        */
        UINT32 getWidth(void) const { return mWidth; }

        /** Gets the height of the image in pixels.
        */
        UINT32 getHeight(void) const { return mHeight; }

        /** Gets the depth of the image.
        */
        UINT32 getDepth(void) const { return mDepth; }

        /** Returns the image format.
        */
        PixelFormat getFormat() const { return mFormat; }

        /** Returns the number of bits per pixel.
        */
        UINT8 getBPP() const { return mBPP; }

        /** Returns true if the image has an alpha component.
        */
        bool getHasAlpha() const { return PixelUtil::getFlags(mFormat) & PFF_HASALPHA; }

		PixelData getPixels(UINT32 mip);

	private:
		UINT32 mNumMipmaps;
		UINT32 mWidth;
		UINT32 mHeight;
		UINT32 mSize;
		UINT32 mDepth;
		INT32 mFlags;
		UINT8 mBPP;
		PixelFormat mFormat;
		UINT8* mData;
	};
}
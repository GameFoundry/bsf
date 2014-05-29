#pragma once

#include "BsCorePrerequisites.h"
#include "BsPixelVolume.h"
#include "BsGpuResourceData.h"
#include "BsIReflectable.h"

namespace BansheeEngine
{
	/**
	 * @brief	The pixel format used for images, textures, and render surfaces
	 */
    enum PixelFormat
    {
        // Unknown pixel format.
        PF_UNKNOWN = 0,
        // 8-bit pixel format, all bits red.
        PF_R8 = 1,
		// 2 byte pixel format, 1 byte red, 1 byte green.
		PF_R8G8 = 2,
        // 24-bit pixel format, 8 bits for red, green and blue.
        PF_R8G8B8 = 3,
        // 24-bit pixel format, 8 bits for blue, green and red.
        PF_B8G8R8 = 4,
        // 32-bit pixel format, 8 bits for alpha, red, green and blue.
        PF_A8R8G8B8 = 5,
        // 32-bit pixel format, 8 bits for blue, green, red and alpha.
        PF_A8B8G8R8 = 6,
        // 32-bit pixel format, 8 bits for blue, green, red and alpha.
        PF_B8G8R8A8 = 7,
		// 32-bit pixel format, 8 bits for red, green, blue and alpha.
		PF_R8G8B8A8 = 8,
        // 32-bit pixel format, 8 bits for red, 8 bits for green, 8 bits for blue.
        // Like PF_A8R8G8B8, but alpha will get discarded
        PF_X8R8G8B8 = 9,
        // 32-bit pixel format, 8 bits for blue, 8 bits for green, 8 bits for red.
        // Like PF_A8B8G8R8, but alpha will get discarded.
        PF_X8B8G8R8 = 10,
		// 32-bit pixel format, 8 bits for red, 8 bits for green, 8 bits for blue.
		// Like PF_R8G8B8A8, but alpha will get discarded.
		PF_R8G8B8X8 = 11,
		// 32-bit pixel format, 8 bits for blue, 8 bits for green, 8 bits for red.
		// Like PF_B8G8R8A8, but alpha will get discarded.
		PF_B8G8R8X8 = 12,
		// 24-bit pixel format, 8 bits for red, green and blue.
		PF_BYTE_RGB = PF_R8G8B8,
		// 24-bit pixel format, 8 bits for blue, green and red.
		PF_BYTE_BGR = PF_B8G8R8,
		// 32-bit pixel format, 8 bits for blue, green, red and alpha.
		PF_BYTE_BGRA = PF_B8G8R8A8,
		// 32-bit pixel format, 8 bits for red, green, blue and alpha.
		PF_BYTE_RGBA = PF_R8G8B8A8,      
        // DXT1/BC1 format containing opaque RGB or 1-bit alpha RGB. 4 bits per pixel.
        PF_DXT1 = 13,
        // DXT2/BC2 format with premultiplied alpha. 6 bits per pixel.
        PF_DXT2 = 14,
        // DXT3/BC2 format with explicit alpha. 6 bits per pixel.
        PF_DXT3 = 15,
        // DXT4/BC3 format with premultiplied alpha. 6 bits per pixel. Better alpha gradients than DXT2.
        PF_DXT4 = 16,
        // DXT5/BC3 format with explicit alpha. 6 bits per pixel. Better alpha gradients than DXT3.
        PF_DXT5 = 17,
		// 16-bit pixel format, 16 bits (float) for red
        PF_FLOAT16_R = 18,
		// 32-bit, 2-channel s10e5 floating point pixel format, 16-bit red, 16-bit green
		PF_FLOAT16_RG = 19,
        // 48-bit pixel format, 16 bits (float) for red, 16 bits (float) for green, 16 bits (float) for blue
        PF_FLOAT16_RGB = 20,
        // 64-bit pixel format, 16 bits (float) for red, 16 bits (float) for green, 16 bits (float) for blue, 16 bits (float) for alpha
        PF_FLOAT16_RGBA = 21,
		// 32-bit pixel format, 32 bits (float) for red
        PF_FLOAT32_R = 22,
		// 64-bit, 2-channel floating point pixel format, 32-bit red, 32-bit green
		PF_FLOAT32_RG = 23,
        // 96-bit pixel format, 32 bits (float) for red, 32 bits (float) for green, 32 bits (float) for blue
        PF_FLOAT32_RGB = 24,
        // 128-bit pixel format, 32 bits (float) for red, 32 bits (float) for green, 32 bits (float) for blue, 32 bits (float) for alpha
        PF_FLOAT32_RGBA = 25,
		// Depth stencil format, 32bit depth, 8bit stencil + 24 unused
		PF_D32_S8X24 = 26,
		// Depth stencil fomrat, 24bit depth + 8bit stencil
		PF_D24S8 = 27,
		// Depth format, 32bits
		PF_D32 = 28,
		// Depth format, 16bits
		PF_D16 = 29,
		// Number of pixel formats currently defined
        PF_COUNT = 30
    };
	typedef Vector<PixelFormat> PixelFormatList;

	/**
	 * @brief	Flags defining some properties of pixel formats.
	 */
    enum PixelFormatFlags {
        // This format has an alpha channel
        PFF_HASALPHA        = 0x00000001,      
        // This format is compressed. This invalidates the values in elemBytes,
        // elemBits and the bit counts as these might not be fixed in a compressed format.
        PFF_COMPRESSED    = 0x00000002,
        // This is a floating point format
        PFF_FLOAT           = 0x00000004,         
        // This is a depth format (for depth textures)
        PFF_DEPTH           = 0x00000008,
        // Format is in native endian. Generally true for the 16, 24 and 32 bits
        // formats which can be represented as machine integers.
        PFF_NATIVEENDIAN    = 0x00000010
    };
    
	/**
	 * @brief	Types of pixel components
	 */
    enum PixelComponentType
    {
        PCT_BYTE = 0,    /**< Byte per component */
        PCT_SHORT = 1,   /**< Short per component */
        PCT_FLOAT16 = 2, /**< 16 bit float per component */
        PCT_FLOAT32 = 3, /**< 32 bit float per component */
        PCT_COUNT = 4    /**< Number of pixel types */
    };
    
	/**
	 * @brief	A buffer describing a volume (3D), image (2D) or line (1D) of pixels in memory.
	 *			Pixels are stored as a succession of "depth" slices, each containing "height" rows of
	 *			"width" pixels.
	 *
	 *			As any GpuResourceData this is used primarily for reading and writing from/to a GPU resource,
	 *			and is normally constructed by the resource itself. However you may still construct it manually
	 *			and use it for other purposes if needed.
	 *
	 * @note	You must call allocateInternalBuffer or set the buffer in some other way before reading/writing
	 *			from this object.
	 *
	 * @see		GpuResourceData
	 */
    class BS_CORE_EXPORT PixelData : public GpuResourceData
	{
    public:
    	PixelData() {}
		~PixelData() {}

		/**
		 * @brief	Constructs a new object with an internal buffer capable of holding "extents" volume of pixels, 
		 *			where each pixel is of the specified pixel format. Extent offsets are also stored, but are not used
		 *			internally.
		 */
		PixelData(const PixelVolume &extents, PixelFormat pixelFormat)
			:mExtents(extents), mFormat(pixelFormat)
		{
			setConsecutive();
		}

		/**
		 * @brief	Constructs a new object with an internal buffer capable of holding volume of pixels described by
		 *			provided width, height and depth, where each pixel is of the specified pixel format.
		 */
    	PixelData(UINT32 width, UINT32 height, UINT32 depth, PixelFormat pixelFormat)
			: mExtents(0, 0, 0, width, height, depth), mFormat(pixelFormat)
    	{
    		setConsecutive();
    	}

		PixelData(const PixelData& copy);
		PixelData& operator=(const PixelData& rhs);

		/**
		 * @brief	Returns the number of pixels that offsets one row from another. This can be
		 *			"width", but doesn't have to be as some buffers require padding.
		 */
		UINT32 getRowPitch() const { return mRowPitch; }

		/**
		 * @brief	Returns the number of pixels that offsets one depth slice from another. This can be
		 *			"width * height", but doesn't have to be as some buffers require padding.
		 */
		UINT32 getSlicePitch() const { return mSlicePitch; }

		/**
		 * @brief	Sets the pitch (in pixels) that determines offset between rows of the pixel buffer.
		 *			Call this before allocating the buffer.
		 */
		void setRowPitch(UINT32 rowPitch) { mRowPitch = rowPitch; }

		/**
		 * @brief	Sets the pitch (in pixels) that determines offset between depth slices of the pixel buffer.
		 *			Call this before allocating the buffer.
		 */
        void setSlicePitch(UINT32 slicePitch) { mSlicePitch = slicePitch; }

		/**
		 * @brief	Returns the number of extra pixels in a row (non-zero only if rows are not
		 *			consecutive (row pitch is larger than width)).
		 */
        UINT32 getRowSkip() const { return mRowPitch - getWidth(); }

		/**
		 * @brief	Returns the number of extra pixels in a depth slice (non-zero only if
		 *			slices aren't consecutive (slice pitch is larger than width*height).
		 */
        UINT32 getSliceSkip() const { return mSlicePitch - (getHeight() * mRowPitch); }

		/**
		 * @brief	Returns the pixel format used by the internal buffer for storing the pixels.
		 */
		PixelFormat getFormat() const { return mFormat; }

		/**
		 * @brief	Returns width of the buffer in pixels.
		 */
		UINT32 getWidth() const { return mExtents.getWidth(); }

		/**
		 * @brief	Returns height of the buffer in pixels.
		 */
		UINT32 getHeight() const { return mExtents.getHeight(); }

		/**
		 * @brief	Returns depth of the buffer in pixels.
		 */
		UINT32 getDepth() const { return mExtents.getDepth(); }

		/**
		 * @brief	Returns left-most start of the pixel volume. This value is not used internally in any way.
		 *			It is just passed through from the constructor.
		 */
		UINT32 getLeft() const { return mExtents.left; }

		/**
		 * @brief	Returns right-most end of the pixel volume. This value is not used internally in any way.
		 *			It is just passed through from the constructor.
		 */
		UINT32 getRight() const { return mExtents.right; }

		/**
		 * @brief	Returns top-most start of the pixel volume. This value is not used internally in any way.
		 *			It is just passed through from the constructor.
		 */
		UINT32 getTop() const { return mExtents.top; }

		/**
		 * @brief	Returns bottom-most end of the pixel volume. This value is not used internally in any way.
		 *			It is just passed through from the constructor.
		 */
		UINT32 getBottom() const { return mExtents.bottom; }

		/**
		 * @brief	Returns front-most start of the pixel volume. This value is not used internally in any way.
		 *			It is just passed through from the constructor.
		 */
		UINT32 getFront() const { return mExtents.front; }

		/**
		 * @brief	Returns back-most end of the pixel volume. This value is not used internally in any way.
		 *			It is just passed through from the constructor.
		 */
		UINT32 getBack() const { return mExtents.back; }

		/**
		 * @brief	Returns extents of the pixel volume this object is capable of holding.
		 */
		PixelVolume getExtents() const { return mExtents; }

		/**
		 * @brief	Return whether this buffer is laid out consecutive in memory 
		 *			(i.e. the pitches are equal to the dimensions).
		 */
        bool isConsecutive() const 
		{ 
			return mRowPitch == getWidth() && mSlicePitch == getWidth()*getHeight(); 
		}

		/**
		 * @brief	Return the size (in bytes) this image would take if it was
		 *			laid out consecutive in memory.
		 */
      	UINT32 getConsecutiveSize() const;

		/**
		 * @brief	Returns pixel data containing a sub-volume of this object. Returned
		 *			data will not have its own buffer, but will instead point to this one.
		 *			It is up to the caller to ensure this object outlives any sub-volume objects.
		 */
      	PixelData getSubVolume(const PixelVolume &def) const;
        
		/**
		 * @brief	Returns pixel color at the specified coordinates.
		 */
        Color getColorAt(UINT32 x, UINT32 y, UINT32 z = 0);

		/**
		 * @brief	Sets the pixel color at the specified coordinates.
		 */
        void setColorAt(Color const &cv, UINT32 x, UINT32 y, UINT32 z = 0);

	private:
		/**
		 * @brief	Set the rowPitch and slicePitch so that the buffer is laid out consecutive
		 *			in memory. Does not actually modify the buffer itself.
		 */
		void setConsecutive()
		{
			mRowPitch = getWidth();
			mSlicePitch = getWidth()*getHeight();
		}

		/**
		 * @brief	Returns the needed size of the internal buffer, in bytes.
		 */
		UINT32 getInternalBufferSize();

	private:
		PixelVolume mExtents;
        PixelFormat mFormat;
        UINT32 mRowPitch;
        UINT32 mSlicePitch;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class PixelDataRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
    };
}
#pragma once

#include "CmPrerequisites.h"
#include "CmBox.h"
#include "CmGpuResourceData.h"
#include "CmIReflectable.h"

namespace CamelotFramework
{
	/** The pixel format used for images, textures, and render surfaces */
    enum PixelFormat
    {
        /// Unknown pixel format.
        PF_UNKNOWN = 0,
        /// 8-bit pixel format, all bits red
        PF_R8 = 1,
		/// 2 byte pixel format, 1 byte red, 1 byte green
		PF_R8G8 = 2,
        /// 24-bit pixel format, 8 bits for red, green and blue.
        PF_R8G8B8 = 3,
        /// 24-bit pixel format, 8 bits for blue, green and red.
        PF_B8G8R8 = 4,
        /// 32-bit pixel format, 8 bits for alpha, red, green and blue.
        PF_A8R8G8B8 = 5,
        /// 32-bit pixel format, 8 bits for blue, green, red and alpha.
        PF_A8B8G8R8 = 6,
        /// 32-bit pixel format, 8 bits for blue, green, red and alpha.
        PF_B8G8R8A8 = 7,
		/// 32-bit pixel format, 8 bits for red, green, blue and alpha.
		PF_R8G8B8A8 = 8,
        /// 32-bit pixel format, 8 bits for red, 8 bits for green, 8 bits for blue
        /// like PF_A8R8G8B8, but alpha will get discarded
        PF_X8R8G8B8 = 9,
        /// 32-bit pixel format, 8 bits for blue, 8 bits for green, 8 bits for red
        /// like PF_A8B8G8R8, but alpha will get discarded
        PF_X8B8G8R8 = 10,
		/// 32-bit pixel format, 8 bits for red, 8 bits for green, 8 bits for blue
		/// like PF_R8G8B8A8, but alpha will get discarded
		PF_R8G8B8X8 = 11,
		/// 32-bit pixel format, 8 bits for blue, 8 bits for green, 8 bits for red
		/// like PF_B8G8R8A8, but alpha will get discarded
		PF_B8G8R8X8 = 12,
		/// 3 byte pixel format, 1 byte for red, 1 byte for green, 1 byte for blue
		PF_BYTE_RGB = PF_R8G8B8,
		/// 3 byte pixel format, 1 byte for blue, 1 byte for green, 1 byte for red
		PF_BYTE_BGR = PF_B8G8R8,
		/// 4 byte pixel format, 1 byte for blue, 1 byte for green, 1 byte for red and one byte for alpha
		PF_BYTE_BGRA = PF_B8G8R8A8,
		/// 4 byte pixel format, 1 byte for red, 1 byte for green, 1 byte for blue, and one byte for alpha
		PF_BYTE_RGBA = PF_R8G8B8A8,      
        /// DDS (DirectDraw Surface) DXT1 format
        PF_DXT1 = 13,
        /// DDS (DirectDraw Surface) DXT2 format
        PF_DXT2 = 14,
        /// DDS (DirectDraw Surface) DXT3 format
        PF_DXT3 = 15,
        /// DDS (DirectDraw Surface) DXT4 format
        PF_DXT4 = 16,
        /// DDS (DirectDraw Surface) DXT5 format
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
		// Depth stencil, 32bit depth, 8bit stencil + 24 unused
		PF_D32_S8X24 = 26,
		// Depth stencil, 24bit depth + 8bit stencil
		PF_D24S8 = 27,
		// Depth, 32bits
		PF_D32 = 28,
		// Depth, 16bits
		PF_D16 = 29,
		// Number of pixel formats currently defined
        PF_COUNT = 30
    };
	typedef vector<PixelFormat>::type PixelFormatList;

    /**
     * Flags defining some on/off properties of pixel formats
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
    
    /** Pixel component format */
    enum PixelComponentType
    {
        PCT_BYTE = 0,    /// Byte per component (8 bit fixed 0.0..1.0)
        PCT_SHORT = 1,   /// Short per component (16 bit fixed 0.0..1.0))
        PCT_FLOAT16 = 2, /// 16 bit float per component
        PCT_FLOAT32 = 3, /// 32 bit float per component
        PCT_COUNT = 4    /// Number of pixel types
    };
    
	/** A primitive describing a volume (3D), image (2D) or line (1D) of pixels in memory.
     	In case of a rectangle, depth must be 1. 
     	Pixels are stored as a succession of "depth" slices, each containing "height" rows of 
     	"width" pixels.
    */
    class CM_EXPORT PixelData : public GpuResourceData
	{
    public:
    	/// Parameter constructor for setting the members manually
    	PixelData() {}
		~PixelData() {}

		/** Constructor providing extents in the form of a Box object. This constructor
    		assumes the pixel data is laid out consecutively in memory. (this
    		means row after row, slice after slice, with no space in between)
    		@param extents	    Extents of the region defined by data
    		@param pixelFormat	Format of this buffer
    		@param pixelData	Pointer to the actual data
    	*/
		PixelData(const Box &extents, PixelFormat pixelFormat)
			:mExtents(extents), mFormat(pixelFormat)
		{
			setConsecutive();
		}

    	/** Constructor providing width, height and depth. This constructor
    		assumes the pixel data is laid out consecutively in memory. (this
    		means row after row, slice after slice, with no space in between)
    		@param width	    Width of the region
    		@param height	    Height of the region
    		@param depth	    Depth of the region
    		@param pixelFormat	Format of this buffer
    		@param pixelData    Pointer to the actual data
    	*/
    	PixelData(UINT32 width, UINT32 height, UINT32 depth, PixelFormat pixelFormat)
			: mExtents(0, 0, 0, width, height, depth), mFormat(pixelFormat)
    	{
    		setConsecutive();
    	}

		PixelData(const PixelData& copy);

		UINT32 getRowPitch() const { return mRowPitch; }
		UINT32 getSlicePitch() const { return mSlicePitch; }

		void setRowPitch(UINT32 rowPitch) { mRowPitch = rowPitch; }
        void setSlicePitch(UINT32 slicePitch) { mSlicePitch = slicePitch; }

        /**	Get the number of elements between one past the rightmost pixel of 
         	one row and the leftmost pixel of the next row. (IE this is zero if rows
         	are consecutive).
        */
        UINT32 getRowSkip() const { return mRowPitch - getWidth(); }
        /** Get the number of elements between one past the right bottom pixel of
         	one slice and the left top pixel of the next slice. (IE this is zero if slices
         	are consecutive).
        */
        UINT32 getSliceSkip() const { return mSlicePitch - (getHeight() * mRowPitch); }

		PixelFormat getFormat() const { return mFormat; }

		UINT32 getWidth() const { return mExtents.getWidth(); }
		UINT32 getHeight() const { return mExtents.getHeight(); }
		UINT32 getDepth() const { return mExtents.getDepth(); }

		UINT32 getLeft() const { return mExtents.left; }
		UINT32 getRight() const { return mExtents.right; }
		UINT32 getTop() const { return mExtents.top; }
		UINT32 getBottom() const { return mExtents.bottom; }
		UINT32 getFront() const { return mExtents.front; }
		UINT32 getBack() const { return mExtents.back; }

		Box getExtents() const { return mExtents; }

        /** Return whether this buffer is laid out consecutive in memory (ie the pitches
         	are equal to the dimensions)
        */        
        bool isConsecutive() const 
		{ 
			return mRowPitch == getWidth() && mSlicePitch == getWidth()*getHeight(); 
		}
        /** Return the size (in bytes) this image would take if it was
        	laid out consecutive in memory
      	*/
      	UINT32 getConsecutiveSize() const;
      	/** Return a subvolume of this PixelBox.
      		@param def	Defines the bounds of the subregion to return
      		@returns	A pixel box describing the region and the data in it
      		@remarks	This function does not copy any data, it just returns
      			a PixelBox object with a data pointer pointing somewhere inside 
      			the data of object.
      		@throws	Exception(ERR_INVALIDPARAMS) if def is not fully contained
      	*/
      	PixelData getSubVolume(const Box &def) const;
        
        /**
         * Get colour value from a certain location in the PixelBox. The z coordinate
         * is only valid for cubemaps and volume textures. This uses the first (largest)
         * mipmap.
         */
        Color getColourAt(UINT32 x, UINT32 y, UINT32 z);

        /**
         * Set colour value at a certain location in the PixelBox. The z coordinate
         * is only valid for cubemaps and volume textures. This uses the first (largest)
         * mipmap.
         */
        void setColourAt(Color const &cv, UINT32 x, UINT32 y, UINT32 z);

	private:
		Box mExtents;

        /// The pixel format 
        PixelFormat mFormat;
        /** Number of elements between the leftmost pixel of one row and the left
         	pixel of the next. This value must always be equal to getWidth() (consecutive) 
			for compressed formats.
        */
        UINT32 mRowPitch;
        /** Number of elements between the top left pixel of one (depth) slice and 
         	the top left pixel of the next. This can be a negative value. Must be a multiple of
         	rowPitch. This value must always be equal to getWidth()*getHeight() (consecutive) 
			for compressed formats.
        */
        UINT32 mSlicePitch;

		/** Set the rowPitch and slicePitch so that the buffer is laid out consecutive 
         	in memory.
        */        
        void setConsecutive()
        {
            mRowPitch = getWidth();
            mSlicePitch = getWidth()*getHeight();
        }

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class PixelDataRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
    };
}
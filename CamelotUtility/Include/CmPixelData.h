#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmBox.h"
#include "CmIReflectable.h"

namespace CamelotEngine
{
	/** The pixel format used for images, textures, and render surfaces */
    enum PixelFormat
    {
        /// Unknown pixel format.
        PF_UNKNOWN = 0,
        /// 8-bit pixel format, all bits luminace.
        PF_L8 = 1,
		PF_BYTE_L = PF_L8,
        /// 16-bit pixel format, all bits luminace.
        PF_L16 = 2,
		PF_SHORT_L = PF_L16,
        /// 8-bit pixel format, all bits alpha.
        PF_A8 = 3,
		PF_BYTE_A = PF_A8,
        /// 8-bit pixel format, 4 bits alpha, 4 bits luminance.
        PF_A4L4 = 4,
		/// 2 byte pixel format, 1 byte red, 1 byte green
		PF_R8G8 = 5,
        /// 16-bit pixel format, 5 bits red, 6 bits green, 5 bits blue.
        PF_R5G6B5 = 6,
		/// 16-bit pixel format, 5 bits red, 6 bits green, 5 bits blue.
        PF_B5G6R5 = 7,
        /// 8-bit pixel format, 2 bits blue, 3 bits green, 3 bits red.
        PF_R3G3B2 = 31,
        /// 16-bit pixel format, 4 bits for alpha, red, green and blue.
        PF_A4R4G4B4 = 8,
        /// 16-bit pixel format, 5 bits for blue, green, red and 1 for alpha.
        PF_A1R5G5B5 = 9,
        /// 24-bit pixel format, 8 bits for red, green and blue.
        PF_R8G8B8 = 10,
        /// 24-bit pixel format, 8 bits for blue, green and red.
        PF_B8G8R8 = 11,
        /// 32-bit pixel format, 8 bits for alpha, red, green and blue.
        PF_A8R8G8B8 = 12,
        /// 32-bit pixel format, 8 bits for blue, green, red and alpha.
        PF_A8B8G8R8 = 13,
        /// 32-bit pixel format, 8 bits for blue, green, red and alpha.
        PF_B8G8R8A8 = 14,
		/// 32-bit pixel format, 8 bits for red, green, blue and alpha.
		PF_R8G8B8A8 = 28,
        /// 32-bit pixel format, 8 bits for red, 8 bits for green, 8 bits for blue
        /// like PF_A8R8G8B8, but alpha will get discarded
        PF_X8R8G8B8 = 26,
        /// 32-bit pixel format, 8 bits for blue, 8 bits for green, 8 bits for red
        /// like PF_A8B8G8R8, but alpha will get discarded
        PF_X8B8G8R8 = 27,
#if CM_ENDIAN == CM_ENDIAN_BIG
		/// 3 byte pixel format, 1 byte for red, 1 byte for green, 1 byte for blue
		PF_BYTE_RGB = PF_R8G8B8,
		/// 3 byte pixel format, 1 byte for blue, 1 byte for green, 1 byte for red
		PF_BYTE_BGR = PF_B8G8R8,
		/// 4 byte pixel format, 1 byte for blue, 1 byte for green, 1 byte for red and one byte for alpha
		PF_BYTE_BGRA = PF_B8G8R8A8,
		/// 4 byte pixel format, 1 byte for red, 1 byte for green, 1 byte for blue, and one byte for alpha
		PF_BYTE_RGBA = PF_R8G8B8A8,
#else
		/// 3 byte pixel format, 1 byte for red, 1 byte for green, 1 byte for blue
		PF_BYTE_RGB = PF_B8G8R8,
		/// 3 byte pixel format, 1 byte for blue, 1 byte for green, 1 byte for red
		PF_BYTE_BGR = PF_R8G8B8,
		/// 4 byte pixel format, 1 byte for blue, 1 byte for green, 1 byte for red and one byte for alpha
		PF_BYTE_BGRA = PF_A8R8G8B8,
		/// 4 byte pixel format, 1 byte for red, 1 byte for green, 1 byte for blue, and one byte for alpha
		PF_BYTE_RGBA = PF_A8B8G8R8,
#endif        
        /// 32-bit pixel format, 2 bits for alpha, 10 bits for red, green and blue.
        PF_A2R10G10B10 = 15,
        /// 32-bit pixel format, 10 bits for blue, green and red, 2 bits for alpha.
        PF_A2B10G10R10 = 16,
        /// DDS (DirectDraw Surface) DXT1 format
        PF_DXT1 = 17,
        /// DDS (DirectDraw Surface) DXT2 format
        PF_DXT2 = 18,
        /// DDS (DirectDraw Surface) DXT3 format
        PF_DXT3 = 19,
        /// DDS (DirectDraw Surface) DXT4 format
        PF_DXT4 = 20,
        /// DDS (DirectDraw Surface) DXT5 format
        PF_DXT5 = 21,
		// 16-bit pixel format, 16 bits (float) for red
        PF_FLOAT16_R = 32,
        // 48-bit pixel format, 16 bits (float) for red, 16 bits (float) for green, 16 bits (float) for blue
        PF_FLOAT16_RGB = 22,
        // 64-bit pixel format, 16 bits (float) for red, 16 bits (float) for green, 16 bits (float) for blue, 16 bits (float) for alpha
        PF_FLOAT16_RGBA = 23,
		// 32-bit pixel format, 32 bits (float) for red
        PF_FLOAT32_R = 33,
        // 96-bit pixel format, 32 bits (float) for red, 32 bits (float) for green, 32 bits (float) for blue
        PF_FLOAT32_RGB = 24,
        // 128-bit pixel format, 32 bits (float) for red, 32 bits (float) for green, 32 bits (float) for blue, 32 bits (float) for alpha
        PF_FLOAT32_RGBA = 25,
		// 32-bit, 2-channel s10e5 floating point pixel format, 16-bit green, 16-bit red
		PF_FLOAT16_GR = 35,
		// 64-bit, 2-channel floating point pixel format, 32-bit green, 32-bit red
		PF_FLOAT32_GR = 36,
		// Depth texture format
		PF_DEPTH = 29,
		// 64-bit pixel format, 16 bits for red, green, blue and alpha
		PF_SHORT_RGBA = 30,
		// 32-bit pixel format, 16-bit green, 16-bit red
		PF_SHORT_GR = 34,
		// 48-bit pixel format, 16 bits for red, green and blue
		PF_SHORT_RGB = 37,
        /// PVRTC (PowerVR) RGB 2 bpp
        PF_PVRTC_RGB2 = 38,
        /// PVRTC (PowerVR) RGBA 2 bpp
        PF_PVRTC_RGBA2 = 39,
        /// PVRTC (PowerVR) RGB 4 bpp
        PF_PVRTC_RGB4 = 40,
        /// PVRTC (PowerVR) RGBA 4 bpp
        PF_PVRTC_RGBA4 = 41,
		// Depth stencil, 32bit depth, 8bit stencil + 24 unused
		PF_D32_S8X24 = 42,
		// Depth stencil, 24bit depth + 8bit stencil
		PF_D24S8 = 43,
		// Depth, 32bits
		PF_D32 = 44,
		// Depth, 16bits
		PF_D16 = 45,
		// Number of pixel formats currently defined
        PF_COUNT = 46
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
        PFF_NATIVEENDIAN    = 0x00000010,
        // This is an intensity format instead of a RGB one. The luminance
        // replaces R,G and B. (but not A)
        PFF_LUMINANCE       = 0x00000020
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
    class CM_UTILITY_EXPORT PixelData: public Box, public IReflectable
	{
    public:
    	/// Parameter constructor for setting the members manually
    	PixelData() {}
		~PixelData() 
		{
			if(ownsData && data != nullptr)
				delete[] data;

			data = nullptr;
		}
		/** Constructor providing extents in the form of a Box object. This constructor
    		assumes the pixel data is laid out consecutively in memory. (this
    		means row after row, slice after slice, with no space in between)
    		@param extents	    Extents of the region defined by data
    		@param pixelFormat	Format of this buffer
    		@param pixelData	Pointer to the actual data
			@param ownsPixelData  If true then PixelData owns the data buffer and will release it when destroyed.
    	*/
		PixelData(const Box &extents, PixelFormat pixelFormat, void *pixelData = 0, bool ownsPixelData = false):
			Box(extents), data(pixelData), format(pixelFormat), ownsData(ownsPixelData)
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
			@param ownsPixelData  If true then PixelData owns the data buffer and will release it when destroyed.
    	*/
    	PixelData(UINT32 width, UINT32 height, UINT32 depth, PixelFormat pixelFormat, 
				void *pixelData = 0, bool ownsPixelData = false):
    		Box(0, 0, 0, width, height, depth),
    		data(pixelData), format(pixelFormat), ownsData(ownsPixelData)
    	{
    		setConsecutive();
    	}

		PixelData(const PixelData& copy);
    	
        /// The data pointer 
        void *data;
        /// The pixel format 
        PixelFormat format;
        /** Number of elements between the leftmost pixel of one row and the left
         	pixel of the next. This value must always be equal to getWidth() (consecutive) 
			for compressed formats.
        */
        UINT32 rowPitch;
        /** Number of elements between the top left pixel of one (depth) slice and 
         	the top left pixel of the next. This can be a negative value. Must be a multiple of
         	rowPitch. This value must always be equal to getWidth()*getHeight() (consecutive) 
			for compressed formats.
        */
        UINT32 slicePitch;

		/**
		 * @brief	If true then PixelData owns the data buffer and will release it when destroyed.
		 */
		bool ownsData;
        
        /** Set the rowPitch and slicePitch so that the buffer is laid out consecutive 
         	in memory.
        */        
        void setConsecutive()
        {
            rowPitch = getWidth();
            slicePitch = getWidth()*getHeight();
        }
        /**	Get the number of elements between one past the rightmost pixel of 
         	one row and the leftmost pixel of the next row. (IE this is zero if rows
         	are consecutive).
        */
        UINT32 getRowSkip() const { return rowPitch - getWidth(); }
        /** Get the number of elements between one past the right bottom pixel of
         	one slice and the left top pixel of the next slice. (IE this is zero if slices
         	are consecutive).
        */
        UINT32 getSliceSkip() const { return slicePitch - (getHeight() * rowPitch); }

		PixelFormat getFormat() const { return format; }

        /** Return whether this buffer is laid out consecutive in memory (ie the pitches
         	are equal to the dimensions)
        */        
        bool isConsecutive() const 
		{ 
			return rowPitch == getWidth() && slicePitch == getWidth()*getHeight(); 
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

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class PixelDataRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
    };
}
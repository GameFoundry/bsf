#pragma once

#include "CmPrerequisites.h"
#include "CmPixelData.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Utility methods for converting and managing pixel data and formats.
	 */
    class BS_CORE_EXPORT PixelUtil 
	{
    public:
		/**
		 * @brief	A list of filtering types to use when scaling images.
		 */
		enum Filter
		{
			FILTER_NEAREST,
			FILTER_LINEAR,
			FILTER_BILINEAR,
			FILTER_BOX,
			FILTER_TRIANGLE,
			FILTER_BICUBIC
		};

		/**
		 * @brief	Returns the size of a single pixel of the provided pixel format,
		 *			in bytes.
		 */
        static UINT32 getNumElemBytes(PixelFormat format);

		/**
		 * @brief	Returns the size of a single pixel of the provided pixel format,
		 *			in bits.
		 */
        static UINT32 getNumElemBits( PixelFormat format );

		/**
		 * @brief	Returns the size of the memory region of the provided size and the pixel format.
		 */
		static UINT32 getMemorySize(UINT32 width, UINT32 height, UINT32 depth, PixelFormat format);
		
		/**
		 * @brief	Returns property flags for this pixel format.
		 *
		 * @see		PixelFormatFlags
		 */
        static UINT32 getFlags(PixelFormat format);

		/**
		 * @brief	Checks if the provided pixel format has an alpha channel.
		 */
        static bool hasAlpha(PixelFormat format);

		/**
		 * @brief	Checks is the provided pixel format a floating point format.
		 */
        static bool isFloatingPoint(PixelFormat format);

		/**
		 * @brief	Checks is the provided pixel format compressed.
		 */
        static bool isCompressed(PixelFormat format);

		/**
		 * @brief	Checks is the provided pixel format a depth/stencil buffer format.
		 */
        static bool isDepth(PixelFormat format);

		/**
		 * @brief	Checks is the provided format in native endian format.
		 */
        static bool isNativeEndian(PixelFormat format);
		
		/**
		 * @brief	Checks are the provided dimensions valid for the specified pixel format.
		 *			Some formats (like DXT) require width/height to be multiples of four and some
		 *			formats dont allow depth larger than 1.
		 */
		static bool isValidExtent(UINT32 width, UINT32 height, UINT32 depth, PixelFormat format);

		/**
		 * @brief	Returns the number of bits per each element in the provided pixel format.
		 *			This will return all zero for compressed and depth/stencil formats.
		 */
        static void getBitDepths(PixelFormat format, int rgba[4]);

		/**
		 * @brief	Returns bit masks that determine in what bit range is each channel stored.
		 *
		 * @note	e.g. if your color is stored in an UINT32 and you want to extract the red channel
		 *			you should AND the color UINT32 with the bit-mask for the red channel and then
		 *			right shift it by the red channel bit shift amount.
		 */
        static void getBitMasks(PixelFormat format, UINT32 rgba[4]);

		/**
		 * @brief	Returns number of bits you need to shift a pixel element in order
		 *			to move it to the start of the data type.
		 *
		 * @note	e.g. if your color is stored in an UINT32 and you want to extract the red channel
		 *			you should AND the color UINT32 with the bit-mask for the red channel and then
		 *			right shift it by the red channel bit shift amount.
		 */
		static void getBitShifts(PixelFormat format, UINT8 rgba[4]);

		/**
		 * @brief	Returns the name of the pixel format.
		 */
        static String getFormatName(PixelFormat srcformat);

		/**
		 * @brief	Returns true if the pixel data in the format can be directly accessed and read.
		 *			This is generally not true for compressed formats.
		 */
        static bool isAccessible(PixelFormat srcformat);
        
		/**
		 * @brief	Returns the type of an individual pixel element in the provided format.
		 */
        static PixelComponentType getElementType(PixelFormat format);
        
		/**
		 * @brief	Returns the number of pixel elements in the provided format.
		 */
		static UINT32 getNumElements(PixelFormat format);

		/**
		 * @brief	Returns the maximum number of mip maps that can be generated until we reach
		 *			the minimum size possible. This does not count the base level.
		 */
		static UINT32 getMaxMipmaps(UINT32 width, UINT32 height, UINT32 depth, PixelFormat format);

		/**
		 * @brief	Writes the color to the provided memory location.
		 */
        static void packColor(const Color& color, PixelFormat format, void* dest);

		/**
		 * @brief	Writes the color to the provided memory location. If the destination
		 *			format is floating point, the byte values will be converted into [0.0, 1.0] range.
		 */
		static void packColor(UINT8 r, UINT8 g, UINT8 b, UINT8 a, PixelFormat format, void* dest);

		/**
		 * @brief	Writes the color to the provided memory location. If the destination format
		 *			in non-floating point, the float values will be assumed to be in [0.0, 1.0] which
		 *			will be converted to integer range. ([0, 255] in the case of bytes)
		 */
		static void packColor(float r, float g, float b, float a, const PixelFormat format, void* dest);

		/**
		 * @brief	Reads the color from the provided memory location and stores it 
		 *			into the provided color object.
		 */
		static void unpackColor(Color* color, PixelFormat format, const void* src);

		/**
		 * @brief	Reads the color from the provided memory location and stores it 
		 *			into the provided color elements, as bytes clamped to [0, 255] range.
		 */
		static void unpackColor(UINT8* r, UINT8* g, UINT8* b, UINT8* a, PixelFormat format, const void* src);

		/**
		 * @brief	Reads the color from the provided memory location and stores it 
		 *			into the provided color elements. If the format is not natively floating
		 *			point a conversion is done in such a way that returned values range [0.0, 1.0].
		 */
        static void unpackColor(float* r, float* g, float* b, float* a, PixelFormat format, const void* src); 
        
		/**
		 * @brief	Converts pixels from one format to another. Provided pixel data objects
		 *			must have previously allocated buffers of adequate size and their sizes must match.
		 */
        static void bulkPixelConversion(const PixelData& src, const PixelData& dst);

		/**
		 * @brief	Scales pixel data in the source buffer and stores the scaled data in the destination buffer.
		 *			Provided pixel data objects must have previously allocated buffers of adequate size. You may
		 *			also provided a filtering method to use when scaling.
		 */
		static void scale(const PixelData& src, const PixelData& dst, Filter filter = FILTER_BILINEAR);

		/**
		 * @brief	Applies gamma correction to the pixels in the provided buffer.
		 *
		 * @param	buffer	Pointer to the buffer containing the pixels.
		 * @param	gamma	Gamma value to apply.
		 * @param	size	Size of the buffer in bytes.
		 * @param	bpp		Number of bits per pixel of the pixels in the buffer.
		 */
        static void applyGamma(UINT8* buffer, float gamma, UINT32 size, UINT8 bpp);
    };
}
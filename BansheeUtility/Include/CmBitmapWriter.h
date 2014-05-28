#pragma once

#include "CmPrerequisitesUtil.h"

namespace BansheeEngine
{
	/**
	 * @brief	Utility class from generating BMP images.
	 */
	class BS_UTILITY_EXPORT BitmapWriter
	{
	public:
		/**
		 * @brief	Generates bytes representing the BMP image format, from a set of raw RGB or RGBA pixels.
		 *
		 * @param	input		  	The input set of bytes in RGB or RGBA format. Starting byte represents the top left pixel of the image
		 * 							and following pixels need to be set going from left to right, row after row.
		 * @param [out]	output	Preallocated buffer where the BMP bytes will be stored. Use "getBMPSize" to retrieve the size needed for this buffer.
		 * @param	width		  	The width of the image in pixels.
		 * @param	height		  	The height of the image in pixels.
		 * @param	bytesPerPixel 	Number of bytes per pixel. 3 for RGB images and 4 for RGBA images. Other values not supported.
		 */
		static void rawPixelsToBMP(const UINT8* input, UINT8* output, UINT32 width, UINT32 height, UINT32 bytesPerPixel);

		/**
		 * @brief	Returns the size of the BMP output buffer that needs to be allocated before calling "rawPixelsToBMP".
		 *
		 * @param	width		  	The width of the image in pixels.
		 * @param	height		  	The height of the image in pixels.
		 * @param	bytesPerPixel 	Number of bytes per pixel. 3 for RGB images and 4 for RGBA images. Other values not supported.
		 *
		 * @return	Size of the BMP output buffer needed to write a BMP of the specified size & bpp.
		 */
		static UINT32 getBMPSize(UINT32 width, UINT32 height, UINT32 bytesPerPixel);
	};
}
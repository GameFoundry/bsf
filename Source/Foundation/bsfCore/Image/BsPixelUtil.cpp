//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Image/BsPixelUtil.h"
#include "Utility/BsBitwise.h"
#include "Image/BsColor.h"
#include "Math/BsMath.h"
#include "Error/BsException.h"
#include "Image/BsTexture.h"
#include <nvtt.h>

namespace bs
{
	/**
	 * Performs pixel data resampling using the point filter (nearest neighbor). Does not perform format conversions.
	 *
	 * @tparam elementSize	Size of a single pixel in bytes.
	 */
	template<UINT32 elementSize> struct NearestResampler
	{
		static void scale(const PixelData& source, const PixelData& dest)
		{
			UINT8* sourceData = source.getData();
			UINT8* destPtr = dest.getData();

			// Get steps for traversing source data in 16/48 fixed point format
			UINT64 stepX = ((UINT64)source.getWidth() << 48) / dest.getWidth();
			UINT64 stepY = ((UINT64)source.getHeight() << 48) / dest.getHeight();
			UINT64 stepZ = ((UINT64)source.getDepth() << 48) / dest.getDepth();

			UINT64 curZ = (stepZ >> 1) - 1; // Offset half a pixel to start at pixel center
			for (UINT32 z = dest.getFront(); z < dest.getBack(); z++, curZ += stepZ)
			{
				UINT32 offsetZ = (UINT32)(curZ >> 48) * source.getSlicePitch();

				UINT64 curY = (stepY >> 1) - 1; // Offset half a pixel to start at pixel center
				for (UINT32 y = dest.getTop(); y < dest.getBottom(); y++, curY += stepY)
				{
					UINT32 offsetY = (UINT32)(curY >> 48) * source.getRowPitch();

					UINT64 curX = (stepX >> 1) - 1; // Offset half a pixel to start at pixel center
					for (UINT32 x = dest.getLeft(); x < dest.getRight(); x++, curX += stepX)
					{
						UINT32 offsetX = (UINT32)(curX >> 48);
						UINT32 offsetBytes = elementSize*(offsetX + offsetY + offsetZ);

						UINT8* curSourcePtr = sourceData + offsetBytes;

						memcpy(destPtr, curSourcePtr, elementSize);
						destPtr += elementSize;
					}

					destPtr += elementSize*dest.getRowSkip();
				}

				destPtr += elementSize*dest.getSliceSkip();
			}
		}
	};

	/** Performs pixel data resampling using the box filter (linear). Performs format conversions. */
	struct LinearResampler
	{
		static void scale(const PixelData& source, const PixelData& dest)
		{
			UINT32 sourceElemSize = PixelUtil::getNumElemBytes(source.getFormat());
			UINT32 destElemSize = PixelUtil::getNumElemBytes(dest.getFormat());

			UINT8* sourceData = source.getData();
			UINT8* destPtr = dest.getData();

			// Get steps for traversing source data in 16/48 fixed point precision format
			UINT64 stepX = ((UINT64)source.getWidth() << 48) / dest.getWidth();
			UINT64 stepY = ((UINT64)source.getHeight() << 48) / dest.getHeight();
			UINT64 stepZ = ((UINT64)source.getDepth() << 48) / dest.getDepth();

			// Contains 16/16 fixed point precision format. Most significant
			// 16 bits will contain the coordinate in the source image, and the
			// least significant 16 bits will contain the fractional part of the coordinate
			// that will be used for determining the blend amount.
			UINT32 temp = 0;

			UINT64 curZ = (stepZ >> 1) - 1; // Offset half a pixel to start at pixel center
			for (UINT32 z = dest.getFront(); z < dest.getBack(); z++, curZ += stepZ)
			{
				temp = UINT32(curZ >> 32);
				temp = (temp > 0x8000)? temp - 0x8000 : 0;
				UINT32 sampleCoordZ1 = temp >> 16;
				UINT32 sampleCoordZ2 = std::min(sampleCoordZ1 + 1, (UINT32)source.getDepth() - 1);
				float sampleWeightZ = (temp & 0xFFFF) / 65536.0f;

				UINT64 curY = (stepY >> 1) - 1; // Offset half a pixel to start at pixel center
				for (UINT32 y = dest.getTop(); y < dest.getBottom(); y++, curY += stepY)
				{
					temp = (UINT32)(curY >> 32);
					temp = (temp > 0x8000)? temp - 0x8000 : 0;
					UINT32 sampleCoordY1 = temp >> 16;
					UINT32 sampleCoordY2 = std::min(sampleCoordY1 + 1, (UINT32)source.getHeight() - 1);
					float sampleWeightY = (temp & 0xFFFF) / 65536.0f;

					UINT64 curX = (stepX >> 1) - 1; // Offset half a pixel to start at pixel center
					for (UINT32 x = dest.getLeft(); x < dest.getRight(); x++, curX += stepX)
					{
						temp = (UINT32)(curX >> 32);
						temp = (temp > 0x8000)? temp - 0x8000 : 0;
						UINT32 sampleCoordX1 = temp >> 16;
						UINT32 sampleCoordX2 = std::min(sampleCoordX1 + 1, (UINT32)source.getWidth() - 1);
						float sampleWeightX = (temp & 0xFFFF) / 65536.0f;

						Color x1y1z1, x2y1z1, x1y2z1, x2y2z1;
						Color x1y1z2, x2y1z2, x1y2z2, x2y2z2;

#define GETSOURCEDATA(x, y, z) sourceData + sourceElemSize*((x)+(y)*source.getRowPitch() + (z)*source.getSlicePitch())

						PixelUtil::unpackColor(&x1y1z1, source.getFormat(), GETSOURCEDATA(sampleCoordX1, sampleCoordY1, sampleCoordZ1));
						PixelUtil::unpackColor(&x2y1z1, source.getFormat(), GETSOURCEDATA(sampleCoordX2, sampleCoordY1, sampleCoordZ1));
						PixelUtil::unpackColor(&x1y2z1, source.getFormat(), GETSOURCEDATA(sampleCoordX1, sampleCoordY2, sampleCoordZ1));
						PixelUtil::unpackColor(&x2y2z1, source.getFormat(), GETSOURCEDATA(sampleCoordX2, sampleCoordY2, sampleCoordZ1));
						PixelUtil::unpackColor(&x1y1z2, source.getFormat(), GETSOURCEDATA(sampleCoordX1, sampleCoordY1, sampleCoordZ2));
						PixelUtil::unpackColor(&x2y1z2, source.getFormat(), GETSOURCEDATA(sampleCoordX2, sampleCoordY1, sampleCoordZ2));
						PixelUtil::unpackColor(&x1y2z2, source.getFormat(), GETSOURCEDATA(sampleCoordX1, sampleCoordY2, sampleCoordZ2));
						PixelUtil::unpackColor(&x2y2z2, source.getFormat(), GETSOURCEDATA(sampleCoordX2, sampleCoordY2, sampleCoordZ2));
#undef GETSOURCEDATA

						Color accum =
							x1y1z1 * ((1.0f - sampleWeightX)*(1.0f - sampleWeightY)*(1.0f - sampleWeightZ)) +
							x2y1z1 * (        sampleWeightX *(1.0f - sampleWeightY)*(1.0f - sampleWeightZ)) +
							x1y2z1 * ((1.0f - sampleWeightX)*        sampleWeightY *(1.0f - sampleWeightZ)) +
							x2y2z1 * (        sampleWeightX *        sampleWeightY *(1.0f - sampleWeightZ)) +
							x1y1z2 * ((1.0f - sampleWeightX)*(1.0f - sampleWeightY)*        sampleWeightZ ) +
							x2y1z2 * (        sampleWeightX *(1.0f - sampleWeightY)*        sampleWeightZ ) +
							x1y2z2 * ((1.0f - sampleWeightX)*        sampleWeightY *        sampleWeightZ ) +
							x2y2z2 * (        sampleWeightX *        sampleWeightY *        sampleWeightZ );

						PixelUtil::packColor(accum, dest.getFormat(), destPtr);

						destPtr += destElemSize;
					}

					destPtr += destElemSize * dest.getRowSkip();
				}

				destPtr += destElemSize * dest.getSliceSkip();
			}
		}
	};


	/**
	 * Performs pixel data resampling using the box filter (linear). Only handles float RGB or RGBA pixel data (32 bits per
	 * channel).
	 */
	struct LinearResampler_Float32
	{
		static void scale(const PixelData& source, const PixelData& dest)
		{
			UINT32 numSourceChannels = PixelUtil::getNumElemBytes(source.getFormat()) / sizeof(float);
			UINT32 numDestChannels = PixelUtil::getNumElemBytes(dest.getFormat()) / sizeof(float);

			float* sourceData = (float*)source.getData();
			float* destPtr = (float*)dest.getData();

			// Get steps for traversing source data in 16/48 fixed point precision format
			UINT64 stepX = ((UINT64)source.getWidth() << 48) / dest.getWidth();
			UINT64 stepY = ((UINT64)source.getHeight() << 48) / dest.getHeight();
			UINT64 stepZ = ((UINT64)source.getDepth() << 48) / dest.getDepth();

			// Contains 16/16 fixed point precision format. Most significant
			// 16 bits will contain the coordinate in the source image, and the
			// least significant 16 bits will contain the fractional part of the coordinate
			// that will be used for determining the blend amount.
			UINT32 temp = 0;

			UINT64 curZ = (stepZ >> 1) - 1; // Offset half a pixel to start at pixel center
			for (UINT32 z = dest.getFront(); z < dest.getBack(); z++, curZ += stepZ)
			{
				temp = (UINT32)(curZ >> 32);
				temp = (temp > 0x8000)? temp - 0x8000 : 0;
				UINT32 sampleCoordZ1 = temp >> 16;
				UINT32 sampleCoordZ2 = std::min(sampleCoordZ1 + 1, (UINT32)source.getDepth() - 1);
				float sampleWeightZ = (temp & 0xFFFF) / 65536.0f;

				UINT64 curY = (stepY >> 1) - 1; // Offset half a pixel to start at pixel center
				for (UINT32 y = dest.getTop(); y < dest.getBottom(); y++, curY += stepY)
				{
					temp = (UINT32)(curY >> 32);
					temp = (temp > 0x8000)? temp - 0x8000 : 0;
					UINT32 sampleCoordY1 = temp >> 16;
					UINT32 sampleCoordY2 = std::min(sampleCoordY1 + 1, (UINT32)source.getHeight() - 1);
					float sampleWeightY = (temp & 0xFFFF) / 65536.0f;

					UINT64 curX = (stepX >> 1) - 1; // Offset half a pixel to start at pixel center
					for (UINT32 x = dest.getLeft(); x < dest.getRight(); x++, curX += stepX)
					{
						temp = (UINT32)(curX >> 32);
						temp = (temp > 0x8000)? temp - 0x8000 : 0;
						UINT32 sampleCoordX1 = temp >> 16;
						UINT32 sampleCoordX2 = std::min(sampleCoordX1 + 1, (UINT32)source.getWidth() - 1);
						float sampleWeightX = (temp & 0xFFFF) / 65536.0f;

						// process R,G,B,A simultaneously for cache coherence?
						float accum[4] = { 0.0f, 0.0f, 0.0f, 0.0f };


#define ACCUM3(x,y,z,factor) \
						{ float f = factor; \
						UINT32 offset = (x + y*source.getRowPitch() + z*source.getSlicePitch())*numSourceChannels; \
						accum[0] += sourceData[offset + 0] * f; accum[1] += sourceData[offset + 1] * f; \
						accum[2] += sourceData[offset + 2] * f; }

#define ACCUM4(x,y,z,factor) \
						{ float f = factor; \
						UINT32 offset = (x + y*source.getRowPitch() + z*source.getSlicePitch())*numSourceChannels; \
						accum[0] += sourceData[offset + 0] * f; accum[1] += sourceData[offset + 1] * f; \
						accum[2] += sourceData[offset + 2] * f; accum[3] += sourceData[offset + 3] * f; }

						if (numSourceChannels == 3 || numDestChannels == 3)
						{
							// RGB
							ACCUM3(sampleCoordX1, sampleCoordY1, sampleCoordZ1, (1.0f - sampleWeightX) * (1.0f - sampleWeightY) * (1.0f - sampleWeightZ));
							ACCUM3(sampleCoordX2, sampleCoordY1, sampleCoordZ1, sampleWeightX		   * (1.0f - sampleWeightY) * (1.0f - sampleWeightZ));
							ACCUM3(sampleCoordX1, sampleCoordY2, sampleCoordZ1, (1.0f - sampleWeightX) * sampleWeightY			* (1.0f - sampleWeightZ));
							ACCUM3(sampleCoordX2, sampleCoordY2, sampleCoordZ1, sampleWeightX		   * sampleWeightY		    * (1.0f - sampleWeightZ));
							ACCUM3(sampleCoordX1, sampleCoordY1, sampleCoordZ2, (1.0f - sampleWeightX) * (1.0f - sampleWeightY) * sampleWeightZ);
							ACCUM3(sampleCoordX2, sampleCoordY1, sampleCoordZ2, sampleWeightX		   * (1.0f - sampleWeightY) * sampleWeightZ);
							ACCUM3(sampleCoordX1, sampleCoordY2, sampleCoordZ2, (1.0f - sampleWeightX) * sampleWeightY			* sampleWeightZ);
							ACCUM3(sampleCoordX2, sampleCoordY2, sampleCoordZ2, sampleWeightX		   * sampleWeightY			* sampleWeightZ);
							accum[3] = 1.0f;
						}
						else
						{
							// RGBA
							ACCUM4(sampleCoordX1, sampleCoordY1, sampleCoordZ1, (1.0f - sampleWeightX) * (1.0f - sampleWeightY) * (1.0f - sampleWeightZ));
							ACCUM4(sampleCoordX2, sampleCoordY1, sampleCoordZ1, sampleWeightX		   * (1.0f - sampleWeightY) * (1.0f - sampleWeightZ));
							ACCUM4(sampleCoordX1, sampleCoordY2, sampleCoordZ1, (1.0f - sampleWeightX) * sampleWeightY			* (1.0f - sampleWeightZ));
							ACCUM4(sampleCoordX2, sampleCoordY2, sampleCoordZ1, sampleWeightX		   * sampleWeightY			* (1.0f - sampleWeightZ));
							ACCUM4(sampleCoordX1, sampleCoordY1, sampleCoordZ2, (1.0f - sampleWeightX) * (1.0f - sampleWeightY) * sampleWeightZ);
							ACCUM4(sampleCoordX2, sampleCoordY1, sampleCoordZ2, sampleWeightX		   * (1.0f - sampleWeightY) * sampleWeightZ);
							ACCUM4(sampleCoordX1, sampleCoordY2, sampleCoordZ2, (1.0f - sampleWeightX) * sampleWeightY			* sampleWeightZ);
							ACCUM4(sampleCoordX2, sampleCoordY2, sampleCoordZ2, sampleWeightX		   * sampleWeightY			* sampleWeightZ);
						}

						memcpy(destPtr, accum, sizeof(float)*numDestChannels);

#undef ACCUM3
#undef ACCUM4

						destPtr += numDestChannels;
					}

					destPtr += numDestChannels*dest.getRowSkip();
				}

				destPtr += numDestChannels*dest.getSliceSkip();
			}
		}
	};



	// byte linear resampler, does not do any format conversions.
	// only handles pixel formats that use 1 byte per color channel.
	// 2D only; punts 3D pixelboxes to default LinearResampler (slow).
	// templated on bytes-per-pixel to allow compiler optimizations, such
	// as unrolling loops and replacing multiplies with bitshifts

	/**
	 * Performs pixel data resampling using the box filter (linear). Only handles pixel formats with one byte per channel.
	 * Does not perform format conversion.
	 *
	 * @tparam	channels	Number of channels in the pixel format.
	 */
	template<UINT32 channels> struct LinearResampler_Byte
	{
		static void scale(const PixelData& source, const PixelData& dest)
		{
			// Only optimized for 2D
			if (source.getDepth() > 1 || dest.getDepth() > 1)
			{
				LinearResampler::scale(source, dest);
				return;
			}

			UINT8* sourceData = (UINT8*)source.getData();
			UINT8* destPtr = (UINT8*)dest.getData();

			// Get steps for traversing source data in 16/48 fixed point precision format
			UINT64 stepX = ((UINT64)source.getWidth() << 48) / dest.getWidth();
			UINT64 stepY = ((UINT64)source.getHeight() << 48) / dest.getHeight();

			// Contains 16/16 fixed point precision format. Most significant
			// 16 bits will contain the coordinate in the source image, and the
			// least significant 16 bits will contain the fractional part of the coordinate
			// that will be used for determining the blend amount.
			UINT32 temp;

			UINT64 curY = (stepY >> 1) - 1; // Offset half a pixel to start at pixel center
			for (UINT32 y = dest.getTop(); y < dest.getBottom(); y++, curY += stepY)
			{
				temp = (UINT32)(curY >> 36);
				temp = (temp > 0x800)? temp - 0x800: 0;
				UINT32 sampleWeightY = temp & 0xFFF;
				UINT32 sampleCoordY1 = temp >> 12;
				UINT32 sampleCoordY2 = std::min(sampleCoordY1 + 1, (UINT32)source.getBottom() - source.getTop() - 1);

				UINT32 sampleY1Offset = sampleCoordY1 * source.getRowPitch();
				UINT32 sampleY2Offset = sampleCoordY2 * source.getRowPitch();

				UINT64 curX = (stepX >> 1) - 1; // Offset half a pixel to start at pixel center
				for (UINT32 x = dest.getLeft(); x < dest.getRight(); x++, curX += stepX)
				{
					temp = (UINT32)(curX >> 36);
					temp = (temp > 0x800)? temp - 0x800 : 0;
					UINT32 sampleWeightX = temp & 0xFFF;
					UINT32 sampleCoordX1 = temp >> 12;
					UINT32 sampleCoordX2 = std::min(sampleCoordX1 + 1, (UINT32)source.getRight() - source.getLeft() - 1);

					UINT32 sxfsyf = sampleWeightX*sampleWeightY;
					for (UINT32 k = 0; k < channels; k++)
					{
						UINT32 accum =
							sourceData[(sampleCoordX1 + sampleY1Offset)*channels+k]*(0x1000000-(sampleWeightX<<12)-(sampleWeightY<<12)+sxfsyf) +
							sourceData[(sampleCoordX2 + sampleY1Offset)*channels+k]*((sampleWeightX<<12)-sxfsyf) +
							sourceData[(sampleCoordX1 + sampleY2Offset)*channels+k]*((sampleWeightY<<12)-sxfsyf) +
							sourceData[(sampleCoordX2 + sampleY2Offset)*channels+k]*sxfsyf;

						// Round up to byte size
						*destPtr = (UINT8)((accum + 0x800000) >> 24);
						destPtr++;
					}
				}
				destPtr += channels*dest.getRowSkip();
			}
		}
	};

	/**	Data describing a pixel format. */
	struct PixelFormatDescription
	{
		const char* name; /**< Name of the format. */
		UINT8 elemBytes; /**< Number of bytes one element (color value) uses. */
		UINT32 flags; /**< PixelFormatFlags set by the pixel format. */
		PixelComponentType componentType; /**< Data type of a single element of the format. */
		UINT8 componentCount; /**< Number of elements in the format. */

		UINT8 rbits, gbits, bbits, abits; /**< Number of bits per element in the format. */

		UINT32 rmask, gmask, bmask, amask; /**< Masks used by packers/unpackers. */
		UINT8 rshift, gshift, bshift, ashift; /**< Shifts used by packers/unpackers. */
	};

	/**	A list of all available pixel formats. */
	PixelFormatDescription _pixelFormats[PF_COUNT] = {
		{"PF_UNKNOWN",
		/* Bytes per element */
		0,
		/* Flags */
		0,
		/* Component type and count */
		PCT_BYTE, 0,
		/* rbits, gbits, bbits, abits */
		0, 0, 0, 0,
		/* Masks and shifts */
		0, 0, 0, 0,
		0, 0, 0, 0,
		},
	//-----------------------------------------------------------------------
		{"PF_R8",
		/* Bytes per element */
		1,
		/* Flags */
		PFF_INTEGER | PFF_NORMALIZED,
		/* Component type and count */
		PCT_BYTE, 1,
		/* rbits, gbits, bbits, abits */
		8, 0, 0, 0,
		/* Masks and shifts */
		0x000000FF, 0, 0, 0,
		0, 0, 0, 0,
		},
	//-----------------------------------------------------------------------
		{"PF_RG8",
		/* Bytes per element */
		2,
		/* Flags */
		PFF_INTEGER | PFF_NORMALIZED,
		/* Component type and count */
		PCT_BYTE, 2,
		/* rbits, gbits, bbits, abits */
		8, 8, 0, 0,
		/* Masks and shifts */
		0x000000FF, 0x0000FF00, 0, 0,
		0, 8, 0, 0,
		},
	//-----------------------------------------------------------------------
		{"PF_RGB8",
		/* Bytes per element */
		4,  // 4th byte is unused
		/* Flags */
		PFF_INTEGER | PFF_NORMALIZED,
		/* Component type and count */
		PCT_BYTE, 3,
		/* rbits, gbits, bbits, abits */
		8, 8, 8, 0,
		/* Masks and shifts */
		0x000000FF, 0x0000FF00, 0x00FF0000, 0,
		0, 8, 16, 0,
		},
	//-----------------------------------------------------------------------
		{"PF_BGR8",
		/* Bytes per element */
		4,  // 4th byte is unused
		/* Flags */
		PFF_INTEGER | PFF_NORMALIZED,
		/* Component type and count */
		PCT_BYTE, 3,
		/* rbits, gbits, bbits, abits */
		8, 8, 8, 0,
		/* Masks and shifts */
		0x00FF0000, 0x0000FF00, 0x000000FF, 0,
		16, 8, 0, 0,
		},
	//-----------------------------------------------------------------------
		{}, // Deleted format
	//-----------------------------------------------------------------------
		{}, // Deleted format
	//-----------------------------------------------------------------------
		{"PF_BGRA8",
		/* Bytes per element */
		4,
		/* Flags */
		PFF_HASALPHA | PFF_INTEGER | PFF_NORMALIZED,
		/* Component type and count */
		PCT_BYTE, 4,
		/* rbits, gbits, bbits, abits */
		8, 8, 8, 8,
		/* Masks and shifts */
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000,
		16, 8, 0, 24,
		},
	//-----------------------------------------------------------------------
		{"PF_RGBA8",
		/* Bytes per element */
		4,
		/* Flags */
		PFF_HASALPHA | PFF_INTEGER | PFF_NORMALIZED,
		/* Component type and count */
		PCT_BYTE, 4,
		/* rbits, gbits, bbits, abits */
		8, 8, 8, 8,
		/* Masks and shifts */
		0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000,
		0, 8, 16, 24,
		},
	//-----------------------------------------------------------------------
		{}, // Deleted format
	//-----------------------------------------------------------------------
		{}, // Deleted format
	//-----------------------------------------------------------------------
		{}, // Deleted format
	//-----------------------------------------------------------------------
		{}, // Deleted format
	//-----------------------------------------------------------------------
		{ "PF_BC1",
		/* Bytes per element */
		0,
		/* Flags */
		PFF_COMPRESSED | PFF_HASALPHA,
		/* Component type and count */
		PCT_BYTE, 3, // No alpha
		/* rbits, gbits, bbits, abits */
		0, 0, 0, 0,
		/* Masks and shifts */
		0, 0, 0, 0,
		0, 0, 0, 0,
		},
	//-----------------------------------------------------------------------
		{ "PF_BC1a",
		/* Bytes per element */
		0,
		/* Flags */
		PFF_COMPRESSED,
		/* Component type and count */
		PCT_BYTE, 3,
		/* rbits, gbits, bbits, abits */
		0, 0, 0, 0,
		/* Masks and shifts */
		0, 0, 0, 0,
		0, 0, 0, 0,
		},
	//-----------------------------------------------------------------------
		{ "PF_BC2",
		/* Bytes per element */
		0,
		/* Flags */
		PFF_COMPRESSED | PFF_HASALPHA,
		/* Component type and count */
		PCT_BYTE, 4,
		/* rbits, gbits, bbits, abits */
		0, 0, 0, 0,
		/* Masks and shifts */
		0, 0, 0, 0,
		0, 0, 0, 0,
		},
	//-----------------------------------------------------------------------
		{ "PF_BC3",
		/* Bytes per element */
		0,
		/* Flags */
		PFF_COMPRESSED | PFF_HASALPHA,
		/* Component type and count */
		PCT_BYTE, 4,
		/* rbits, gbits, bbits, abits */
		0, 0, 0, 0,
		/* Masks and shifts */
		0, 0, 0, 0,
		0, 0, 0, 0,
		},
	//-----------------------------------------------------------------------
		{ "PF_BC4",
		/* Bytes per element */
		0,
		/* Flags */
		PFF_COMPRESSED,
		/* Component type and count */
		PCT_BYTE, 1,
		/* rbits, gbits, bbits, abits */
		0, 0, 0, 0,
		/* Masks and shifts */
		0, 0, 0, 0,
		0, 0, 0, 0,
		},
	//-----------------------------------------------------------------------
		{ "PF_BC5",
		/* Bytes per element */
		0,
		/* Flags */
		PFF_COMPRESSED,
		/* Component type and count */
		PCT_BYTE, 2,
		/* rbits, gbits, bbits, abits */
		0, 0, 0, 0,
		/* Masks and shifts */
		0, 0, 0, 0,
		0, 0, 0, 0,
		},
	//-----------------------------------------------------------------------
		{ "PF_BC6H",
		/* Bytes per element */
		0,
		/* Flags */
		PFF_COMPRESSED,
		/* Component type and count */
		PCT_FLOAT16, 3,
		/* rbits, gbits, bbits, abits */
		0, 0, 0, 0,
		/* Masks and shifts */
		0, 0, 0, 0,
		0, 0, 0, 0,
		},
	//-----------------------------------------------------------------------
		{ "PF_BC7",
		/* Bytes per element */
		0,
		/* Flags */
		PFF_COMPRESSED | PFF_HASALPHA,
		/* Component type and count */
		PCT_BYTE, 4,
		/* rbits, gbits, bbits, abits */
		0, 0, 0, 0,
		/* Masks and shifts */
		0, 0, 0, 0,
		0, 0, 0, 0,
		},
	//-----------------------------------------------------------------------
		{"PF_R16F",
		/* Bytes per element */
		2,
		/* Flags */
		PFF_FLOAT,
		/* Component type and count */
		PCT_FLOAT16, 1,
		/* rbits, gbits, bbits, abits */
		16, 0, 0, 0,
		/* Masks and shifts */
		0x0000FFFF, 0, 0, 0,
		0, 0, 0, 0,
		},
	//-----------------------------------------------------------------------
		{"PF_RG16F",
		/* Bytes per element */
		4,
		/* Flags */
		PFF_FLOAT,
		/* Component type and count */
		PCT_FLOAT16, 2,
		/* rbits, gbits, bbits, abits */
		16, 16, 0, 0,
		/* Masks and shifts */
		0x0000FFFF, 0xFFFF0000, 0, 0,
		0, 16, 0, 0,
		},
	//-----------------------------------------------------------------------
		{ }, //  Deleted format
	//-----------------------------------------------------------------------
		{ "PF_RGBA16F",
		/* Bytes per element */
		8,
		/* Flags */
		PFF_FLOAT | PFF_HASALPHA,
		/* Component type and count */
		PCT_FLOAT16, 4,
		/* rbits, gbits, bbits, abits */
		16, 16, 16, 16,
		/* Masks and shifts */
		0x0000FFFF, 0xFFFF0000, 0x0000FFFF, 0xFFFF0000,
		0, 16, 0, 16,
		},
	//-----------------------------------------------------------------------
		{"PF_R32F",
		/* Bytes per element */
		4,
		/* Flags */
		PFF_FLOAT,
		/* Component type and count */
		PCT_FLOAT32, 1,
		/* rbits, gbits, bbits, abits */
		32, 0, 0, 0,
		/* Masks and shifts */
		0xFFFFFFFF, 0, 0, 0,
		0, 0, 0, 0,
		},
	//-----------------------------------------------------------------------
		{"PF_RG32F",
		/* Bytes per element */
		8,
		/* Flags */
		PFF_FLOAT,
		/* Component type and count */
		PCT_FLOAT32, 2,
		/* rbits, gbits, bbits, abits */
		32, 32, 0, 0,
		/* Masks and shifts */
		0xFFFFFFFF, 0xFFFFFFFF, 0, 0,
		0, 0, 0, 0,
		},
	//-----------------------------------------------------------------------
		{ "PF_RGB32F",
		/* Bytes per element */
		12,
		/* Flags */
		PFF_FLOAT,
		/* Component type and count */
		PCT_FLOAT32, 3,
		/* rbits, gbits, bbits, abits */
		32, 32, 32, 0,
		/* Masks and shifts */
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0,
		0, 0, 0, 0,
		},
	//-----------------------------------------------------------------------
		{ "PF_RGBA32F",
		/* Bytes per element */
		16,
		/* Flags */
		PFF_FLOAT | PFF_HASALPHA,
		/* Component type and count */
		PCT_FLOAT32, 4,
		/* rbits, gbits, bbits, abits */
		32, 32, 32, 32,
		/* Masks and shifts */
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0, 0, 0, 0,
		},
	//-----------------------------------------------------------------------
		{"PF_D32_S8X24",
		/* Bytes per element */
		8,
		/* Flags */
		PFF_DEPTH | PFF_NORMALIZED,
		/* Component type and count */
		PCT_FLOAT32, 2,
		/* rbits, gbits, bbits, abits */
		32, 8, 0, 0,
		/* Masks and shifts */
		0xFFFFFFFF, 0x000000FF, 0x00000000, 0x00000000,
		0, 0, 0, 0,
		},
	//-----------------------------------------------------------------------
		{"PF_D24_S8",
		/* Bytes per element */
		4,
		/* Flags */
		PFF_DEPTH | PFF_INTEGER | PFF_NORMALIZED,
		/* Component type and count */
		PCT_INT, 2,
		/* rbits, gbits, bbits, abits */
		24, 8, 0, 0,
		/* Masks and shifts */
		0x00FFFFFF, 0x0FF0000, 0x00000000, 0x00000000,
		0, 24, 0, 0,
		},
	//-----------------------------------------------------------------------
		{"PF_D32",
		/* Bytes per element */
		4,
		/* Flags */
		PFF_DEPTH | PFF_FLOAT,
		/* Component type and count */
		PCT_FLOAT32, 1,
		/* rbits, gbits, bbits, abits */
		32, 0, 0, 0,
		/* Masks and shifts */
		0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000,
		0, 0, 0, 0,
		},
	//-----------------------------------------------------------------------
		{"PF_D16",
		/* Bytes per element */
		2,
		/* Flags */
		PFF_DEPTH | PFF_INTEGER | PFF_NORMALIZED,
		/* Component type and count */
		PCT_SHORT, 1,
		/* rbits, gbits, bbits, abits */
		16, 0, 0, 0,
		/* Masks and shifts */
		0x0000FFFF, 0x00000000, 0x00000000, 0x00000000,
		0, 0, 0, 0,
		},
	//-----------------------------------------------------------------------
		{ "PF_RG11B10F",
		/* Bytes per element */
		4,
		/* Flags */
		PFF_FLOAT,
		/* Component type and count */
		PCT_PACKED_R11G11B10, 1,
		/* rbits, gbits, bbits, abits */
		11, 11, 10, 0,
		/* Masks and shifts */
		0x000007FF, 0x003FF800, 0xFFC00000, 0,
		0, 11, 22, 0,
		},
	//-----------------------------------------------------------------------
		{ "PF_RGB10A2",
		/* Bytes per element */
		4,
		/* Flags */
		PFF_INTEGER | PFF_NORMALIZED | PFF_HASALPHA,
		/* Component type and count */
		PCT_PACKED_R10G10B10A2, 1,
		/* rbits, gbits, bbits, abits */
		10, 10, 10, 2,
		/* Masks and shifts */
		0x000003FF, 0x000FFC00, 0x3FF00000, 0xC0000000,
		0, 10, 20, 30,
		},
	//-----------------------------------------------------------------------
		{ "PF_R8I",
		/* Bytes per element */
		1,
		/* Flags */
		PFF_INTEGER | PFF_SIGNED,
		/* Component type and count */
		PCT_BYTE, 1,
		/* rbits, gbits, bbits, abits */
		8, 0, 0, 0,
		/* Masks and shifts */
		0x000000FF, 0, 0, 0,
		0, 0, 0, 0,
		},
	//-----------------------------------------------------------------------
		{ "PF_RG8I",
		/* Bytes per element */
		2,
		/* Flags */
		PFF_INTEGER | PFF_SIGNED,
		/* Component type and count */
		PCT_BYTE, 2,
		/* rbits, gbits, bbits, abits */
		8, 8, 0, 0,
		/* Masks and shifts */
		0x000000FF, 0x0000FF00, 0, 0,
		0, 8, 0, 0,
		},
	//-----------------------------------------------------------------------
		{ "PF_RGBA8I",
		/* Bytes per element */
		4,
		/* Flags */
		PFF_INTEGER | PFF_SIGNED | PFF_HASALPHA,
		/* Component type and count */
		PCT_BYTE, 4,
		/* rbits, gbits, bbits, abits */
		8, 8, 8, 8,
		/* Masks and shifts */
		0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000,
		0, 8, 16, 24,
		},
	//-----------------------------------------------------------------------
		{ "PF_R8U",
		/* Bytes per element */
		1,
		/* Flags */
		PFF_INTEGER,
		/* Component type and count */
		PCT_BYTE, 1,
		/* rbits, gbits, bbits, abits */
		8, 0, 0, 0,
		/* Masks and shifts */
		0x000000FF, 0, 0, 0,
		0, 0, 0, 0,
		},
	//-----------------------------------------------------------------------
		{ "PF_RG8U",
		/* Bytes per element */
		2,
		/* Flags */
		PFF_INTEGER,
		/* Component type and count */
		PCT_BYTE, 2,
		/* rbits, gbits, bbits, abits */
		8, 8, 0, 0,
		/* Masks and shifts */
		0x000000FF, 0x0000FF00, 0, 0,
		0, 8, 0, 0,
		},
	//-----------------------------------------------------------------------
		{ "PF_RGBA8U",
		/* Bytes per element */
		4,
		/* Flags */
		PFF_INTEGER | PFF_HASALPHA,
		/* Component type and count */
		PCT_BYTE, 4,
		/* rbits, gbits, bbits, abits */
		8, 8, 8, 8,
		/* Masks and shifts */
		0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000,
		0, 8, 16, 24,
		},
	//-----------------------------------------------------------------------
		{ "PF_R8S",
		/* Bytes per element */
		1,
		/* Flags */
		PFF_INTEGER | PFF_NORMALIZED | PFF_SIGNED,
		/* Component type and count */
		PCT_BYTE, 1,
		/* rbits, gbits, bbits, abits */
		8, 0, 0, 0,
		/* Masks and shifts */
		0x000000FF, 0, 0, 0,
		0, 0, 0, 0,
		},
	//-----------------------------------------------------------------------
		{ "PF_RG8S",
		/* Bytes per element */
		2,
		/* Flags */
		PFF_INTEGER | PFF_NORMALIZED | PFF_SIGNED,
		/* Component type and count */
		PCT_BYTE, 2,
		/* rbits, gbits, bbits, abits */
		8, 8, 0, 0,
		/* Masks and shifts */
		0x000000FF, 0x0000FF00, 0, 0,
		0, 8, 0, 0,
		},
	//-----------------------------------------------------------------------
		{ "PF_RGBA8S",
		/* Bytes per element */
		4,
		/* Flags */
		PFF_INTEGER | PFF_NORMALIZED | PFF_SIGNED | PFF_HASALPHA,
		/* Component type and count */
		PCT_BYTE, 4,
		/* rbits, gbits, bbits, abits */
		8, 8, 8, 8,
		/* Masks and shifts */
		0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000,
		0, 8, 16, 24,
		},
	//-----------------------------------------------------------------------
		{ "PF_R16I",
		/* Bytes per element */
		2,
		/* Flags */
		PFF_INTEGER | PFF_SIGNED,
		/* Component type and count */
		PCT_SHORT, 1,
		/* rbits, gbits, bbits, abits */
		16, 0, 0, 0,
		/* Masks and shifts */
		0x0000FFFF, 0, 0, 0,
		0, 0, 0, 0,
		},
	//-----------------------------------------------------------------------
		{ "PF_RG16I",
		/* Bytes per element */
		4,
		/* Flags */
		PFF_INTEGER | PFF_SIGNED,
		/* Component type and count */
		PCT_SHORT, 2,
		/* rbits, gbits, bbits, abits */
		16, 16, 0, 0,
		/* Masks and shifts */
		0x0000FFFF, 0xFFFF0000, 0, 0,
		0, 16, 0, 0,
		},
	//-----------------------------------------------------------------------
		{ "PF_RGBA16I",
		/* Bytes per element */
		8,
		/* Flags */
		PFF_INTEGER | PFF_SIGNED | PFF_HASALPHA,
		/* Component type and count */
		PCT_SHORT, 4,
		/* rbits, gbits, bbits, abits */
		16, 16, 16, 16,
		/* Masks and shifts */
		0x0000FFFF, 0xFFFF0000, 0x0000FFFF, 0xFFFF0000,
		0, 16, 0, 16,
		},
	//-----------------------------------------------------------------------
		{ "PF_R16U",
		/* Bytes per element */
		2,
		/* Flags */
		PFF_INTEGER,
		/* Component type and count */
		PCT_SHORT, 1,
		/* rbits, gbits, bbits, abits */
		16, 0, 0, 0,
		/* Masks and shifts */
		0x0000FFFF, 0, 0, 0,
		0, 0, 0, 0,
		},
	//-----------------------------------------------------------------------
		{ "PF_RG16U",
		/* Bytes per element */
		4,
		/* Flags */
		PFF_INTEGER,
		/* Component type and count */
		PCT_SHORT, 2,
		/* rbits, gbits, bbits, abits */
		16, 16, 0, 0,
		/* Masks and shifts */
		0x0000FFFF, 0xFFFF0000, 0, 0,
		0, 16, 0, 0,
		},
	//-----------------------------------------------------------------------
		{ "PF_RGBA16U",
		/* Bytes per element */
		8,
		/* Flags */
		PFF_INTEGER | PFF_HASALPHA,
		/* Component type and count */
		PCT_SHORT, 4,
		/* rbits, gbits, bbits, abits */
		16, 16, 16, 16,
		/* Masks and shifts */
		0x0000FFFF, 0xFFFF0000, 0x0000FFFF, 0xFFFF0000,
		0, 16, 0, 16,
		},
	//-----------------------------------------------------------------------
		{ "PF_R32I",
		/* Bytes per element */
		4,
		/* Flags */
		PFF_INTEGER,
		/* Component type and count */
		PCT_INT, 1,
		/* rbits, gbits, bbits, abits */
		32, 0, 0, 0,
		/* Masks and shifts */
		0xFFFFFFFF, 0, 0, 0,
		0, 0, 0, 0,
		},
	//-----------------------------------------------------------------------
		{ "PF_RG32I",
		/* Bytes per element */
		8,
		/* Flags */
		PFF_INTEGER | PFF_SIGNED,
		/* Component type and count */
		PCT_INT, 2,
		/* rbits, gbits, bbits, abits */
		32, 32, 0, 0,
		/* Masks and shifts */
		0xFFFFFFFF, 0xFFFFFFFF, 0, 0,
		0, 0, 0, 0,
		},
	//-----------------------------------------------------------------------
		{ "PF_RGB32I",
		/* Bytes per element */
		12,
		/* Flags */
		PFF_INTEGER | PFF_SIGNED,
		/* Component type and count */
		PCT_INT, 3,
		/* rbits, gbits, bbits, abits */
		32, 32, 32, 0,
		/* Masks and shifts */
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0,
		0, 0, 0, 0,
		},
	//-----------------------------------------------------------------------
		{ "PF_RGBA32I",
		/* Bytes per element */
		16,
		/* Flags */
		PFF_INTEGER | PFF_SIGNED | PFF_HASALPHA,
		/* Component type and count */
		PCT_INT, 4,
		/* rbits, gbits, bbits, abits */
		32, 32, 32, 32,
		/* Masks and shifts */
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0, 0, 0, 0
		},
	//-----------------------------------------------------------------------
		{ "PF_R32U",
		/* Bytes per element */
		4,
		/* Flags */
		PFF_INTEGER,
		/* Component type and count */
		PCT_INT, 1,
		/* rbits, gbits, bbits, abits */
		32, 0, 0, 0,
		/* Masks and shifts */
		0xFFFFFFFF, 0, 0, 0,
		0, 0, 0, 0,
		},
	//-----------------------------------------------------------------------
		{ "PF_RG32U",
		/* Bytes per element */
		8,
		/* Flags */
		PFF_INTEGER,
		/* Component type and count */
		PCT_INT, 2,
		/* rbits, gbits, bbits, abits */
		32, 32, 0, 0,
		/* Masks and shifts */
		0xFFFFFFFF, 0xFFFFFFFF, 0, 0,
		0, 0, 0, 0,
		},
	//-----------------------------------------------------------------------
		{ "PF_RGB32U",
		/* Bytes per element */
		12,
		/* Flags */
		PFF_INTEGER,
		/* Component type and count */
		PCT_INT, 3,
		/* rbits, gbits, bbits, abits */
		32, 32, 32, 0,
		/* Masks and shifts */
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0,
		0, 0, 0, 0,
		},
	//-----------------------------------------------------------------------
		{ "PF_RGBA32U",
		/* Bytes per element */
		16,
		/* Flags */
		PFF_INTEGER | PFF_HASALPHA,
		/* Component type and count */
		PCT_INT, 4,
		/* rbits, gbits, bbits, abits */
		32, 32, 32, 32,
		/* Masks and shifts */
		0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
		0, 0, 0, 0
		},
	//-----------------------------------------------------------------------
		{ "PF_R16S",
		/* Bytes per element */
		2,
		/* Flags */
		PFF_INTEGER | PFF_NORMALIZED | PFF_SIGNED,
		/* Component type and count */
		PCT_SHORT, 1,
		/* rbits, gbits, bbits, abits */
		16, 0, 0, 0,
		/* Masks and shifts */
		0x0000FFFF, 0, 0, 0,
		0, 0, 0, 0,
		},
	//-----------------------------------------------------------------------
		{ "PF_RG16S",
		/* Bytes per element */
		4,
		/* Flags */
		PFF_INTEGER | PFF_NORMALIZED | PFF_SIGNED,
		/* Component type and count */
		PCT_SHORT, 2,
		/* rbits, gbits, bbits, abits */
		16, 16, 0, 0,
		/* Masks and shifts */
		0x0000FFFF, 0xFFFF0000, 0, 0,
		0, 16, 0, 0,
		},
	//-----------------------------------------------------------------------
		{ "PF_RGBA16S",
		/* Bytes per element */
		8,
		/* Flags */
		PFF_INTEGER | PFF_NORMALIZED | PFF_SIGNED | PFF_HASALPHA,
		/* Component type and count */
		PCT_SHORT, 4,
		/* rbits, gbits, bbits, abits */
		16, 16, 16, 16,
		/* Masks and shifts */
		0x0000FFFF, 0xFFFF0000, 0x0000FFFF, 0xFFFF0000,
		0, 16, 0, 16,
		},
	//-----------------------------------------------------------------------
		{ "PF_R16",
		/* Bytes per element */
		2,
		/* Flags */
		PFF_INTEGER | PFF_NORMALIZED,
		/* Component type and count */
		PCT_SHORT, 1,
		/* rbits, gbits, bbits, abits */
		16, 0, 0, 0,
		/* Masks and shifts */
		0x0000FFFF, 0, 0, 0,
		0, 0, 0, 0
		},
	//-----------------------------------------------------------------------
		{ "PF_RG16",
		/* Bytes per element */
		4,
		/* Flags */
		PFF_INTEGER | PFF_NORMALIZED,
		/* Component type and count */
		PCT_SHORT, 2,
		/* rbits, gbits, bbits, abits */
		16, 16, 0, 0,
		/* Masks and shifts */
		0x0000FFFF, 0xFFFF0000, 0, 0,
		0, 16, 0, 0
		},
	//-----------------------------------------------------------------------
		{ "PF_RGBA16",
		/* Bytes per element */
		8,
		/* Flags */
		PFF_INTEGER | PFF_NORMALIZED | PFF_HASALPHA,
		/* Component type and count */
		PCT_SHORT, 4,
		/* rbits, gbits, bbits, abits */
		16, 16, 16, 16,
		/* Masks and shifts */
		0x0000FFFF, 0xFFFF0000, 0x0000FFFF, 0xFFFF0000,
		0, 16, 0, 16
		},
	};

	static inline const PixelFormatDescription &getDescriptionFor(const PixelFormat fmt)
	{
		const int ord = (int)fmt;
		assert(ord >= 0 && ord < PF_COUNT);

		return _pixelFormats[ord];
	}

	/**	Handles compression output from NVTT library for a single image. */
	struct NVTTCompressOutputHandler : public nvtt::OutputHandler
	{
		NVTTCompressOutputHandler(UINT8* buffer, UINT32 sizeBytes)
			:buffer(buffer), bufferWritePos(buffer), bufferEnd(buffer + sizeBytes)
		{ }

		void beginImage(int size, int width, int height, int depth, int face, int miplevel) override
		{ }

		bool writeData(const void* data, int size) override
		{
			assert((bufferWritePos + size) <= bufferEnd);
			memcpy(bufferWritePos, data, size);
			bufferWritePos += size;

			return true;
		}

		void endImage() override
		{ }

		UINT8* buffer;
		UINT8* bufferWritePos;
		UINT8* bufferEnd;
	};

	/**	Handles output from NVTT library for a mip-map chain. */
	struct NVTTMipmapOutputHandler : public nvtt::OutputHandler
	{
		NVTTMipmapOutputHandler(const Vector<SPtr<PixelData>>& buffers)
			:buffers(buffers), bufferWritePos(nullptr), bufferEnd(nullptr)
		{ }

		void beginImage(int size, int width, int height, int depth, int face, int miplevel) override
		{
			assert(miplevel >= 0 && miplevel < (int)buffers.size());
			assert((UINT32)size == buffers[miplevel]->getConsecutiveSize());

			activeBuffer = buffers[miplevel];

			bufferWritePos = activeBuffer->getData();
			bufferEnd = bufferWritePos + activeBuffer->getConsecutiveSize();
		}

		bool writeData(const void* data, int size) override
		{
			assert((bufferWritePos + size) <= bufferEnd);
			memcpy(bufferWritePos, data, size);
			bufferWritePos += size;

			return true;
		}

		void endImage() override
		{ }

		Vector<SPtr<PixelData>> buffers;
		SPtr<PixelData> activeBuffer;

		UINT8* bufferWritePos;
		UINT8* bufferEnd;
	};

	nvtt::Format toNVTTFormat(PixelFormat format)
	{
		switch (format)
		{
		case PF_BC1:
			return nvtt::Format_BC1;
		case PF_BC1a:
			return nvtt::Format_BC1a;
		case PF_BC2:
			return nvtt::Format_BC2;
		case PF_BC3:
			return nvtt::Format_BC3;
		case PF_BC4:
			return nvtt::Format_BC4;
		case PF_BC5:
			return nvtt::Format_BC5;
		case PF_BC6H:
			return nvtt::Format_BC6;
		case PF_BC7:
			return nvtt::Format_BC7;
		default: // Unsupported format
			return nvtt::Format_BC3;
		}
	}

	nvtt::Quality toNVTTQuality(CompressionQuality quality)
	{
		switch (quality)
		{
		case CompressionQuality::Fastest:
			return nvtt::Quality_Fastest;
		case CompressionQuality::Highest:
			return nvtt::Quality_Highest;
		case CompressionQuality::Normal:
			return nvtt::Quality_Normal;
		case CompressionQuality::Production:
			return nvtt::Quality_Normal;
		}

		// Unknown quality level
		return nvtt::Quality_Normal;
	}

	nvtt::AlphaMode toNVTTAlphaMode(AlphaMode alphaMode)
	{
		switch (alphaMode)
		{
		case AlphaMode::None:
			return nvtt::AlphaMode_None;
		case AlphaMode::Premultiplied:
			return nvtt::AlphaMode_Premultiplied;
		case AlphaMode::Transparency:
			return nvtt::AlphaMode_Transparency;
		}

		// Unknown alpha mode
		return nvtt::AlphaMode_None;
	}

	nvtt::WrapMode toNVTTWrapMode(MipMapWrapMode wrapMode)
	{
		switch (wrapMode)
		{
		case MipMapWrapMode::Clamp:
			return nvtt::WrapMode_Clamp;
		case MipMapWrapMode::Mirror:
			return nvtt::WrapMode_Mirror;
		case MipMapWrapMode::Repeat:
			return nvtt::WrapMode_Repeat;
		}

		// Unknown alpha mode
		return nvtt::WrapMode_Mirror;
	}

	UINT32 PixelUtil::getNumElemBytes(PixelFormat format)
	{
		return getDescriptionFor(format).elemBytes;
	}

	UINT32 PixelUtil::getMemorySize(UINT32 width, UINT32 height, UINT32 depth, PixelFormat format)
	{
		if(isCompressed(format))
		{
			switch(format)
			{
				// BC formats work by dividing the image into 4x4 blocks, then encoding each
				// 4x4 block with a certain number of bytes.
				case PF_BC1:
				case PF_BC1a:
				case PF_BC4:
					return ((width+3)/4)*((height+3)/4)*8 * depth;
				case PF_BC2:
				case PF_BC3:
				case PF_BC5:
				case PF_BC6H:
				case PF_BC7:
					return ((width+3)/4)*((height+3)/4)*16 * depth;

				default:
					BS_EXCEPT(InvalidParametersException, "Invalid compressed pixel format");
					return 0;
			}
		}

		return width*height*depth*getNumElemBytes(format);
	}

	void PixelUtil::getPitch(UINT32 width, UINT32 height, UINT32 depth, PixelFormat format,
						 UINT32& rowPitch, UINT32& depthPitch)
	{
		if (isCompressed(format))
		{
			switch (format)
			{
				// BC formats work by dividing the image into 4x4 blocks, then encoding each
				// 4x4 block with a certain number of bytes.
			case PF_BC1:
			case PF_BC1a:
			case PF_BC4:
			case PF_BC2:
			case PF_BC3:
			case PF_BC5:
			case PF_BC6H:
			case PF_BC7:
				rowPitch = div(width + 3, 4).quot * 4;
				depthPitch = div(height + 3, 4).quot * 4 * rowPitch;
				return;

			default:
				BS_EXCEPT(InvalidParametersException, "Invalid compressed pixel format");
				return;
			}
		}

		rowPitch = width;
		depthPitch = width * height;
	}

	void PixelUtil::getSizeForMipLevel(UINT32 width, UINT32 height, UINT32 depth, UINT32 mipLevel,
		UINT32& mipWidth, UINT32& mipHeight, UINT32& mipDepth)
	{
		mipWidth = width;
		mipHeight = height;
		mipDepth = depth;

		for (UINT32 i = 0; i < mipLevel; i++)
		{
			if (mipWidth != 1) mipWidth /= 2;
			if (mipHeight != 1) mipHeight /= 2;
			if (mipDepth != 1) mipDepth /= 2;
		}
	}

	UINT32 PixelUtil::getNumElemBits(PixelFormat format)
	{
		return getDescriptionFor(format).elemBytes * 8;
	}

	UINT32 PixelUtil::getFlags(PixelFormat format)
	{
		return getDescriptionFor(format).flags;
	}

	bool PixelUtil::hasAlpha(PixelFormat format)
	{
		return (PixelUtil::getFlags(format) & PFF_HASALPHA) > 0;
	}

	bool PixelUtil::isFloatingPoint(PixelFormat format)
	{
		return (PixelUtil::getFlags(format) & PFF_FLOAT) > 0;
	}

	bool PixelUtil::isCompressed(PixelFormat format)
	{
		return (PixelUtil::getFlags(format) & PFF_COMPRESSED) > 0;
	}

	bool PixelUtil::isNormalized(PixelFormat format)
	{
		return (PixelUtil::getFlags(format) & PFF_NORMALIZED) > 0;
	}

	bool PixelUtil::isDepth(PixelFormat format)
	{
		return (PixelUtil::getFlags(format) & PFF_DEPTH) > 0;
	}

	bool PixelUtil::checkFormat(PixelFormat& format, TextureType texType, int usage)
	{
		// First check just the usage since it's the most limiting factor

		//// Depth-stencil only supports depth formats
		if ((usage & TU_DEPTHSTENCIL) != 0)
		{
			if (isDepth(format))
				return true;

			format = PF_D32_S8X24;
			return false;
		}

		//// Render targets support everything but compressed & depth-stencil formats
		if ((usage & TU_RENDERTARGET) != 0)
		{
			if (!isDepth(format) && !isCompressed(format))
				return true;

			format = PF_RGBA8;
			return false;
		}

		//// Load-store textures support everything but compressed & depth-stencil formats
		if ((usage & TU_LOADSTORE) != 0)
		{
			if (!isDepth(format) && !isCompressed(format))
				return true;

			format = PF_RGBA8;
			return false;
		}

		//// Sampled texture support depends on texture type
		switch (texType)
		{
		case TEX_TYPE_1D:
		{
			// 1D textures support anything but depth & compressed formats
			if (!isDepth(format) && !isCompressed(format))
				return true;

			format = PF_RGBA8;
			return false;
		}
		case TEX_TYPE_3D:
		{
			// 3D textures support anything but depth & compressed formats
			if (!isDepth(format))
				return true;

			format = PF_RGBA8;
			return false;
		}
		default: // 2D & cube
		{
			// 2D/cube textures support anything but depth formats
			if (!isDepth(format))
				return true;

			format = PF_RGBA8;
			return false;
		}
		}
	}

	bool PixelUtil::isValidExtent(UINT32 width, UINT32 height, UINT32 depth, PixelFormat format)
	{
		if(isCompressed(format))
		{
			switch(format)
			{
				case PF_BC1:
				case PF_BC2:
				case PF_BC1a:
				case PF_BC3:
				case PF_BC4:
				case PF_BC5:
				case PF_BC6H:
				case PF_BC7:
					return ((width & 3) == 0 && (height & 3) == 0 && depth == 1);
				default:
					return true;
			}
		}
		else
		{
			return true;
		}
	}

	void PixelUtil::getBitDepths(PixelFormat format, int(&rgba)[4])
	{
		const PixelFormatDescription& des = getDescriptionFor(format);
		rgba[0] = des.rbits;
		rgba[1] = des.gbits;
		rgba[2] = des.bbits;
		rgba[3] = des.abits;
	}

	void PixelUtil::getBitMasks(PixelFormat format, UINT32(&rgba)[4])
	{
		const PixelFormatDescription& des = getDescriptionFor(format);
		rgba[0] = des.rmask;
		rgba[1] = des.gmask;
		rgba[2] = des.bmask;
		rgba[3] = des.amask;
	}

	void PixelUtil::getBitShifts(PixelFormat format, UINT8(&rgba)[4])
	{
		const PixelFormatDescription& des = getDescriptionFor(format);
		rgba[0] = des.rshift;
		rgba[1] = des.gshift;
		rgba[2] = des.bshift;
		rgba[3] = des.ashift;
	}

	String PixelUtil::getFormatName(PixelFormat srcformat)
	{
		return getDescriptionFor(srcformat).name;
	}

	bool PixelUtil::isAccessible(PixelFormat srcformat)
	{
		if (srcformat == PF_UNKNOWN)
			return false;

		UINT32 flags = getFlags(srcformat);
		return !((flags & PFF_COMPRESSED) || (flags & PFF_DEPTH));
	}

	PixelComponentType PixelUtil::getElementType(PixelFormat format)
	{
		const PixelFormatDescription& des = getDescriptionFor(format);
		return des.componentType;
	}

	UINT32 PixelUtil::getNumElements(PixelFormat format)
	{
		const PixelFormatDescription& des = getDescriptionFor(format);
		return des.componentCount;
	}

	UINT32 PixelUtil::getMaxMipmaps(UINT32 width, UINT32 height, UINT32 depth, PixelFormat format)
	{
		UINT32 count = 0;
		if ((width > 0) && (height > 0))
		{
			while (!(width == 1 && height == 1 && depth == 1))
			{
				if (width > 1)		width = width / 2;
				if (height > 1)	height = height / 2;
				if (depth > 1)		depth = depth / 2;

				count++;
			}
		}

		return count;
	}

	void PixelUtil::packColor(const Color& color, PixelFormat format, void* dest)
	{
		packColor(color.r, color.g, color.b, color.a, format, dest);
	}

	void PixelUtil::packColor(UINT8 r, UINT8 g, UINT8 b, UINT8 a, PixelFormat format, void* dest)
	{
		const PixelFormatDescription &des = getDescriptionFor(format);

		if (des.flags & PFF_INTEGER)
		{
			// Shortcut for integer formats packing
			UINT32 value = ((Bitwise::fixedToFixed(r, 8, des.rbits) << des.rshift) & des.rmask) |
				((Bitwise::fixedToFixed(g, 8, des.gbits) << des.gshift) & des.gmask) |
				((Bitwise::fixedToFixed(b, 8, des.bbits) << des.bshift) & des.bmask) |
				((Bitwise::fixedToFixed(a, 8, des.abits) << des.ashift) & des.amask);

			// And write to memory
			Bitwise::intWrite(dest, des.elemBytes, value);
		}
		else
		{
			// Convert to float
			packColor((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f, format, dest);
		}
	}

	void PixelUtil::packColor(float r, float g, float b, float a, const PixelFormat format, void* dest)
	{
		// Special cases
		if (format == PF_RG11B10F)
		{
			UINT32 value;
			value = Bitwise::floatToFloat11(r);
			value |= Bitwise::floatToFloat11(g) << 11;
			value |= Bitwise::floatToFloat10(b) << 22;

			((UINT32*)dest)[0] = value;
			return;
		}

		if (format == PF_RGB10A2)
		{
			LOGERR("packColor() not implemented for format \"" + getFormatName(PF_RGB10A2) + "\".");
			return;
		}

		// All other formats handled in a generic way
		const PixelFormatDescription& des = getDescriptionFor(format);
		assert(des.componentCount <= 4);

		float inputs[] = { r, g, b, a };
		UINT8 bits[] = { des.rbits, des.gbits, des.bbits, des.abits };
		UINT32 masks[] = { des.rmask, des.gmask, des.bmask, des.amask };
		UINT8 shifts[] = { des.rshift, des.gshift, des.bshift, des.ashift };

		memset(dest, 0, des.elemBytes);

		UINT32 curBit = 0;
		UINT32 prevDword = 0;
		UINT32 dwordValue = 0;
		for (UINT32 i = 0; i < des.componentCount; i++)
		{
			UINT32 curDword = curBit / 32;

			// New dword reached, write current one and advance
			if(curDword > prevDword)
			{
				UINT32* curDst = ((UINT32*)dest) + prevDword;
				Bitwise::intWrite(curDst, 4, dwordValue);

				dwordValue = 0;
				prevDword = curDword;
			}

			if (des.flags & PFF_INTEGER)
			{
				if (des.flags & PFF_NORMALIZED)
				{
					if (des.flags & PFF_SIGNED)
						dwordValue |= (Bitwise::snormToUint(inputs[i], bits[i]) << shifts[i]) & masks[i];
					else
						dwordValue |= (Bitwise::unormToUint(inputs[i], bits[i]) << shifts[i]) & masks[i];
				}
				else
				{
					// Note: Casting integer to float. A better option would be to have a separate unpackColor that has
					// integer output parameters.
					dwordValue |= (((UINT32)inputs[i]) << shifts[i]) & masks[i];
				}
			}
			else if (des.flags & PFF_FLOAT)
			{
				// Note: Not handling unsigned floats

				if (des.componentType == PCT_FLOAT16)
					dwordValue |= (Bitwise::floatToHalf(inputs[i]) << shifts[i]) & masks[i];
				else
					dwordValue |= *(UINT32*)&inputs[i];
			}
			else
			{
				LOGERR("packColor() not implemented for format \"" + getFormatName(format) + "\".");
				return;
			}

			curBit += bits[i];
		}

		// Write last dword
		UINT32 numBytes = std::min((prevDword + 1) * 4, (UINT32)des.elemBytes) - (prevDword * 4);
		UINT32* curDst = ((UINT32*)dest) + prevDword;
		Bitwise::intWrite(curDst, numBytes, dwordValue);
	}

	void PixelUtil::unpackColor(Color* color, PixelFormat format, const void* src)
	{
		unpackColor(&color->r, &color->g, &color->b, &color->a, format, src);
	}

	void PixelUtil::unpackColor(UINT8* r, UINT8* g, UINT8* b, UINT8* a, PixelFormat format, const void* src)
	{
		const PixelFormatDescription &des = getDescriptionFor(format);

		if (des.flags & PFF_INTEGER)
		{
			// Shortcut for integer formats unpacking
			const UINT32 value = Bitwise::intRead(src, des.elemBytes);

			*r = (UINT8)Bitwise::fixedToFixed((value & des.rmask) >> des.rshift, des.rbits, 8);
			*g = (UINT8)Bitwise::fixedToFixed((value & des.gmask) >> des.gshift, des.gbits, 8);
			*b = (UINT8)Bitwise::fixedToFixed((value & des.bmask) >> des.bshift, des.bbits, 8);

			if (des.flags & PFF_HASALPHA)
			{
				*a = (UINT8)Bitwise::fixedToFixed((value & des.amask) >> des.ashift, des.abits, 8);
			}
			else
			{
				*a = 255; // No alpha, default a component to full
			}
		}
		else
		{
			// Do the operation with the more generic floating point
			float rr, gg, bb, aa;
			unpackColor(&rr, &gg, &bb, &aa, format, src);

			*r = (UINT8)Bitwise::unormToUint(rr, 8);
			*g = (UINT8)Bitwise::unormToUint(gg, 8);
			*b = (UINT8)Bitwise::unormToUint(bb, 8);
			*a = (UINT8)Bitwise::unormToUint(aa, 8);
		}
	}

	void PixelUtil::unpackColor(float* r, float* g, float* b, float* a, PixelFormat format, const void* src)
	{
		// Special cases
		if(format == PF_RG11B10F)
		{
			UINT32 value = ((UINT32*)src)[0];
			*r = Bitwise::float11ToFloat(value);
			*g = Bitwise::float11ToFloat(value >> 11);
			*b = Bitwise::float10ToFloat(value >> 22);

			return;
		}

		if(format == PF_RGB10A2)
		{
			LOGERR("unpackColor() not implemented for format \"" + getFormatName(PF_RGB10A2) + "\".");
			return;
		}

		// All other formats handled in a generic way
		const PixelFormatDescription& des = getDescriptionFor(format);
		assert(des.componentCount <= 4);

		float* outputs[] = { r, g, b, a };
		UINT8 bits[] = { des.rbits, des.gbits, des.bbits, des.abits };
		UINT32 masks[] = { des.rmask, des.gmask, des.bmask, des.amask };
		UINT8 shifts[] = { des.rshift, des.gshift, des.bshift, des.ashift };

		UINT32 curBit = 0;
		for(UINT32 i = 0; i < des.componentCount; i++)
		{
			UINT32 curDword = curBit / 32;
			UINT32 numBytes = std::min((curDword + 1) * 4, (UINT32)des.elemBytes) - (curDword * 4);

			UINT32* curSrc = ((UINT32*)src) + curDword;
			UINT32 value = Bitwise::intRead(curSrc, numBytes);
			if(des.flags & PFF_INTEGER)
			{
				if(des.flags & PFF_NORMALIZED)
				{
					if (des.flags & PFF_SIGNED)
						*outputs[i] = Bitwise::uintToSnorm((value & masks[i]) >> shifts[i], bits[i]);
					else
						*outputs[i] = Bitwise::uintToUnorm((value & masks[i]) >> shifts[i], bits[i]);
				}
				else
				{
					// Note: Casting integer to float. A better option would be to have a separate unpackColor that has
					// integer output parameters.
					*outputs[i] = (float)((value & masks[i]) >> shifts[i]);
				}
			}
			else if(des.flags & PFF_FLOAT)
			{
				// Note: Not handling unsigned floats

				if (des.componentType == PCT_FLOAT16)
					*outputs[i] = Bitwise::halfToFloat((UINT16)((value & masks[i]) >> shifts[i]));
				else
					*outputs[i] = *(float*)&value;
			}
			else
			{
				LOGERR("unpackColor() not implemented for format \"" + getFormatName(format) + "\".");
				return;
			}

			curBit += bits[i];
		}

		// Fill empty components
		for (UINT32 i = des.componentCount; i < 3; i++)
			*outputs[i] = 0.0f;

		if (des.componentCount < 4)
			*outputs[3] = 1.0f;
	}

	void PixelUtil::packDepth(float depth, const PixelFormat format, void* dest)
	{
		if (!isDepth(format))
		{
			LOGERR("Cannot convert depth to " + getFormatName(format) + ": it is not a depth format");
			return;
		}

		LOGERR("Method is not implemented");
		//TODO implement depth packing
	}

	float PixelUtil::unpackDepth(PixelFormat format, void* src)
	{
		if (!isDepth(format))
		{
			LOGERR("Cannot unpack from " + getFormatName(format) + ": it is not a depth format");
			return 0;
		}

		UINT32* color = (UINT32 *)src;
		UINT32 masked = 0;
		switch (format)
		{
		case PF_D24S8:
			return  static_cast<float>(*color & 0x00FFFFFF) / (float)16777216;
			break;
		case PF_D16:
			return static_cast<float>(*color & 0xFFFF) / (float)65536;
			break;
		case PF_D32:
			masked = *color & 0xFFFFFFFF;
			return *((float*)&masked);
			break;
		case PF_D32_S8X24:
			masked = *color & 0xFFFFFFFF;
			return *( (float*) &masked );
			break;
		default:
			LOGERR("Cannot unpack from " + getFormatName(format));
			return 0;
			break;
		}
	}

	void PixelUtil::bulkPixelConversion(const PixelData &src, PixelData &dst)
	{
		assert(src.getWidth() == dst.getWidth() &&
			src.getHeight() == dst.getHeight() &&
			src.getDepth() == dst.getDepth());

		// Check for compressed formats, we don't support decompression
		if (PixelUtil::isCompressed(src.getFormat()))
		{
			if (src.getFormat() == dst.getFormat())
			{
				memcpy(dst.getData(), src.getData(), src.getConsecutiveSize());
				return;
			}
			else
			{
				LOGERR("bulkPixelConversion() cannot be used to compress or decompress images");
				return;
			}
		}

		// Check for compression
		if (PixelUtil::isCompressed(dst.getFormat()))
		{
			if (src.getFormat() == dst.getFormat())
			{
				memcpy(dst.getData(), src.getData(), src.getConsecutiveSize());
				return;
			}
			else
			{
				CompressionOptions co;
				co.format = dst.getFormat();
				compress(src, dst, co);

				return;
			}
		}

		// The easy case
		if (src.getFormat() == dst.getFormat())
		{
			// Everything consecutive?
			if (src.isConsecutive() && dst.isConsecutive())
			{
				memcpy(dst.getData(), src.getData(), src.getConsecutiveSize());
				return;
			}

			const UINT32 srcPixelSize = PixelUtil::getNumElemBytes(src.getFormat());
			const UINT32 dstPixelSize = PixelUtil::getNumElemBytes(dst.getFormat());
			UINT8 *srcptr = static_cast<UINT8*>(src.getData())
				+ (src.getLeft() + src.getTop() * src.getRowPitch() + src.getFront() * src.getSlicePitch()) * srcPixelSize;
			UINT8 *dstptr = static_cast<UINT8*>(dst.getData())
				+ (dst.getLeft() + dst.getTop() * dst.getRowPitch() + dst.getFront() * dst.getSlicePitch()) * dstPixelSize;

			// Calculate pitches+skips in bytes
			const UINT32 srcRowPitchBytes = src.getRowPitch()*srcPixelSize;
			const UINT32 srcSliceSkipBytes = src.getSliceSkip()*srcPixelSize;

			const UINT32 dstRowPitchBytes = dst.getRowPitch()*dstPixelSize;
			const UINT32 dstSliceSkipBytes = dst.getSliceSkip()*dstPixelSize;

			// Otherwise, copy per row
			const UINT32 rowSize = src.getWidth()*srcPixelSize;
			for (UINT32 z = src.getFront(); z < src.getBack(); z++)
			{
				for (UINT32 y = src.getTop(); y < src.getBottom(); y++)
				{
					memcpy(dstptr, srcptr, rowSize);

					srcptr += srcRowPitchBytes;
					dstptr += dstRowPitchBytes;
				}

				srcptr += srcSliceSkipBytes;
				dstptr += dstSliceSkipBytes;
			}

			return;
		}

		UINT32 srcPixelSize = PixelUtil::getNumElemBytes(src.getFormat());
		UINT32 dstPixelSize = PixelUtil::getNumElemBytes(dst.getFormat());
		UINT8 *srcptr = static_cast<UINT8*>(src.getData())
			+ (src.getLeft() + src.getTop() * src.getRowPitch() + src.getFront() * src.getSlicePitch()) * srcPixelSize;
		UINT8 *dstptr = static_cast<UINT8*>(dst.getData())
			+ (dst.getLeft() + dst.getTop() * dst.getRowPitch() + dst.getFront() * dst.getSlicePitch()) * dstPixelSize;

		// Calculate pitches+skips in bytes
		UINT32 srcRowSkipBytes = src.getRowSkip()*srcPixelSize;
		UINT32 srcSliceSkipBytes = src.getSliceSkip()*srcPixelSize;
		UINT32 dstRowSkipBytes = dst.getRowSkip()*dstPixelSize;
		UINT32 dstSliceSkipBytes = dst.getSliceSkip()*dstPixelSize;

		// The brute force fallback
		float r, g, b, a;
		for (UINT32 z = src.getFront(); z < src.getBack(); z++)
		{
			for (UINT32 y = src.getTop(); y < src.getBottom(); y++)
			{
				for (UINT32 x = src.getLeft(); x < src.getRight(); x++)
				{
					unpackColor(&r, &g, &b, &a, src.getFormat(), srcptr);
					packColor(r, g, b, a, dst.getFormat(), dstptr);

					srcptr += srcPixelSize;
					dstptr += dstPixelSize;
				}

				srcptr += srcRowSkipBytes;
				dstptr += dstRowSkipBytes;
			}

			srcptr += srcSliceSkipBytes;
			dstptr += dstSliceSkipBytes;
		}
	}

	void PixelUtil::flipComponentOrder(PixelData& data)
	{
		if (isCompressed(data.getFormat()))
		{
			LOGERR("flipComponentOrder() not supported on compressed images.");
			return;
		}

		const PixelFormatDescription& pfd = getDescriptionFor(data.getFormat());
		if(pfd.elemBytes > 4)
		{
			LOGERR("flipComponentOrder() only supported on 4 byte or smaller pixel formats.");
			return;
		}

		if (pfd.componentCount <= 1) // Nothing to flip
			return;

		bool bitCountMismatch = false;
		if (pfd.rbits != pfd.gbits)
			bitCountMismatch = true;

		if(pfd.componentCount > 2 && pfd.rbits != pfd.bbits)
			bitCountMismatch = true;

		if (pfd.componentCount > 3 && pfd.rbits != pfd.abits)
			bitCountMismatch = true;

		if(bitCountMismatch)
		{
			LOGERR("flipComponentOrder() not supported for formats that don't have the same number of bytes for all components.");
			return;
		}

		struct CompData
		{
			UINT32 mask;
			UINT8 shift;
		};

		std::array<CompData, 4> compData =
		{{
			{ pfd.rmask, pfd.rshift },
			{ pfd.gmask, pfd.gshift },
			{ pfd.bmask, pfd.bshift },
			{ pfd.amask, pfd.ashift }
		}};

		// Ensure unused components are at the end, after sort
		if (pfd.componentCount < 4)
			compData[3].shift = 0xFF;

		if (pfd.componentCount < 3)
			compData[2].shift = 0xFF;

		std::sort(compData.begin(), compData.end(),
			[&](const CompData& lhs, const CompData& rhs) { return lhs.shift < rhs.shift; }
		);

		UINT8* dataPtr = data.getData();

		UINT32 pixelSize = pfd.elemBytes;
		UINT32 rowSkipBytes = data.getRowSkip()*pixelSize;
		UINT32 sliceSkipBytes = data.getSliceSkip()*pixelSize;

		for (UINT32 z = 0; z < data.getDepth(); z++)
		{
			for (UINT32 y = 0; y < data.getHeight(); y++)
			{
				for (UINT32 x = 0; x < data.getWidth(); x++)
				{
					if(pfd.componentCount == 2)
					{
						UINT64 pixelData = 0;
						memcpy(&pixelData, dataPtr, pixelSize);

						UINT64 output = 0;
						output |= (pixelData & compData[1].mask) >> compData[1].shift;
						output |= (pixelData & compData[0].mask) << compData[1].shift;

						memcpy(dataPtr, &output, pixelSize);
					}
					else if(pfd.componentCount == 3)
					{
						UINT64 pixelData = 0;
						memcpy(&pixelData, dataPtr, pixelSize);

						UINT64 output = 0;
						output |= (pixelData & compData[2].mask) >> compData[2].shift;
						output |= (pixelData & compData[0].mask) << compData[2].shift;

						memcpy(dataPtr, &output, pixelSize);
					}
					else if(pfd.componentCount == 4)
					{
						UINT64 pixelData = 0;
						memcpy(&pixelData, dataPtr, pixelSize);

						UINT64 output = 0;
						output |= (pixelData & compData[3].mask) >> compData[3].shift;
						output |= (pixelData & compData[0].mask) << compData[3].shift;

						output |= (pixelData & compData[2].mask) >> (compData[2].shift - compData[1].shift);
						output |= (pixelData & compData[1].mask) << (compData[2].shift - compData[1].shift);

						memcpy(dataPtr, &output, pixelSize);
					}

					dataPtr += pixelSize;
				}

				dataPtr += rowSkipBytes;
			}

			dataPtr += sliceSkipBytes;
		}
	}

	void PixelUtil::scale(const PixelData& src, PixelData& scaled, Filter filter)
	{
		assert(PixelUtil::isAccessible(src.getFormat()));
		assert(PixelUtil::isAccessible(scaled.getFormat()));

		PixelData temp;
		switch (filter)
		{
		default:
		case FILTER_NEAREST:
			if(src.getFormat() == scaled.getFormat())
			{
				// No intermediate buffer needed
				temp = scaled;
			}
			else
			{
				// Allocate temporary buffer of destination size in source format
				temp = PixelData(scaled.getWidth(), scaled.getHeight(), scaled.getDepth(), src.getFormat());
				temp.allocateInternalBuffer();
			}

			// No conversion
			switch (PixelUtil::getNumElemBytes(src.getFormat()))
			{
			case 1: NearestResampler<1>::scale(src, temp); break;
			case 2: NearestResampler<2>::scale(src, temp); break;
			case 3: NearestResampler<3>::scale(src, temp); break;
			case 4: NearestResampler<4>::scale(src, temp); break;
			case 6: NearestResampler<6>::scale(src, temp); break;
			case 8: NearestResampler<8>::scale(src, temp); break;
			case 12: NearestResampler<12>::scale(src, temp); break;
			case 16: NearestResampler<16>::scale(src, temp); break;
			default:
				// Never reached
				assert(false);
			}

			if(temp.getData() != scaled.getData())
			{
				// Blit temp buffer
				PixelUtil::bulkPixelConversion(temp, scaled);

				temp.freeInternalBuffer();
			}

			break;

		case FILTER_LINEAR:
			switch (src.getFormat())
			{
			case PF_RG8:
			case PF_RGB8: case PF_BGR8:
			case PF_RGBA8: case PF_BGRA8:
				if(src.getFormat() == scaled.getFormat())
				{
					// No intermediate buffer needed
					temp = scaled;
				}
				else
				{
					// Allocate temp buffer of destination size in source format
					temp = PixelData(scaled.getWidth(), scaled.getHeight(), scaled.getDepth(), src.getFormat());
					temp.allocateInternalBuffer();
				}

				// No conversion
				switch (PixelUtil::getNumElemBytes(src.getFormat()))
				{
				case 1: LinearResampler_Byte<1>::scale(src, temp); break;
				case 2: LinearResampler_Byte<2>::scale(src, temp); break;
				case 3: LinearResampler_Byte<3>::scale(src, temp); break;
				case 4: LinearResampler_Byte<4>::scale(src, temp); break;
				default:
					// Never reached
					assert(false);
				}

				if(temp.getData() != scaled.getData())
				{
					// Blit temp buffer
					PixelUtil::bulkPixelConversion(temp, scaled);
					temp.freeInternalBuffer();
				}

				break;
			case PF_RGB32F:
			case PF_RGBA32F:
				if (scaled.getFormat() == PF_RGB32F || scaled.getFormat() == PF_RGBA32F)
				{
					// float32 to float32, avoid unpack/repack overhead
					LinearResampler_Float32::scale(src, scaled);
					break;
				}
				// Else, fall through
			default:
				// Fallback case, slow but works
				LinearResampler::scale(src, scaled);
			}
			break;
		}
	}

	void PixelUtil::copy(const PixelData& src, PixelData& dst, UINT32 offsetX, UINT32 offsetY, UINT32 offsetZ)
	{
		if(src.getFormat() != dst.getFormat())
		{
			LOGERR("Source format is different from destination format for copy(). This operation cannot be used for "
				   "a format conversion. Aborting copy.");
			return;
		}

		UINT32 right = offsetX + dst.getWidth();
		UINT32 bottom = offsetY + dst.getHeight();
		UINT32 back = offsetZ + dst.getDepth();

		if(right > src.getWidth() || bottom > src.getHeight() || back > src.getDepth())
		{
			LOGERR("Provided offset or destination size is too large and is referencing pixels that are out of bounds"
				   " on the source texture. Aborting copy().");
			return;
		}

		UINT8* srcPtr = (UINT8*)src.getData() + offsetZ * src.getSlicePitch();
		UINT8* dstPtr = (UINT8*)dst.getData();

		UINT32 elemSize = getNumElemBytes(dst.getFormat());
		UINT32 rowSize = dst.getWidth() * elemSize;

		for(UINT32 z = 0; z < dst.getDepth(); z++)
		{
			UINT8* srcRowPtr = srcPtr + offsetY * src.getRowPitch() * elemSize;
			UINT8* dstRowPtr = dstPtr;

			for(UINT32 y = 0; y < dst.getHeight(); y++)
			{
				memcpy(dstRowPtr, srcRowPtr + offsetX * elemSize, rowSize);

				srcRowPtr += src.getRowPitch() * elemSize;
				dstRowPtr += dst.getRowPitch() * elemSize;
			}

			srcPtr += src.getSlicePitch() * elemSize;
			dstPtr += dst.getSlicePitch() * elemSize;
		}
	}

	void PixelUtil::mirror(PixelData& pixelData, MirrorMode mode)
	{
		UINT32 width = pixelData.getWidth();
		UINT32 height = pixelData.getHeight();
		UINT32 depth = pixelData.getDepth();

		UINT32 elemSize = getNumElemBytes(pixelData.getFormat());

		if (mode.isSet(MirrorModeBits::Z))
		{
			UINT32 sliceSize = width * height * elemSize;
			UINT8* sliceTemp = bs_stack_alloc<UINT8>(sliceSize);

			UINT8* dataPtr = pixelData.getData();
			UINT32 halfDepth = depth / 2;
			for (UINT32 z = 0; z < halfDepth; z++)
			{
				UINT32 srcZ = z * sliceSize;
				UINT32 dstZ = (depth - z - 1) * sliceSize;

				memcpy(sliceTemp, &dataPtr[dstZ], sliceSize);
				memcpy(&dataPtr[dstZ], &dataPtr[srcZ], sliceSize);
				memcpy(&dataPtr[srcZ], sliceTemp, sliceSize);
			}

			// Note: If flipping Y or X as well I could do it here without an extra set of memcpys

			bs_stack_free(sliceTemp);
		}

		if(mode.isSet(MirrorModeBits::Y))
		{
			UINT32 rowSize = width * elemSize;
			UINT8* rowTemp = bs_stack_alloc<UINT8>(rowSize);

			UINT8* slicePtr = pixelData.getData();
			for (UINT32 z = 0; z < depth; z++)
			{
				UINT32 halfHeight = height / 2;
				for (UINT32 y = 0; y < halfHeight; y++)
				{
					UINT32 srcY = y * rowSize;
					UINT32 dstY = (height - y - 1) * rowSize;

					memcpy(rowTemp, &slicePtr[dstY], rowSize);
					memcpy(&slicePtr[dstY], &slicePtr[srcY], rowSize);
					memcpy(&slicePtr[srcY], rowTemp, rowSize);
				}

				// Note: If flipping X as well I could do it here without an extra set of memcpys

				slicePtr += pixelData.getSlicePitch() * elemSize;
			}

			bs_stack_free(rowTemp);
		}

		if (mode.isSet(MirrorModeBits::X))
		{
			UINT8* elemTemp = bs_stack_alloc<UINT8>(elemSize);

			UINT8* slicePtr = pixelData.getData();
			for (UINT32 z = 0; z < depth; z++)
			{
				UINT8* rowPtr = slicePtr;
				for (UINT32 y = 0; y < height; y++)
				{
					UINT32 halfWidth = width / 2;
					for (UINT32 x = 0; x < halfWidth; x++)
					{
						UINT32 srcX = x * elemSize;
						UINT32 dstX = (width - x - 1) * elemSize;

						memcpy(elemTemp, &rowPtr[dstX], elemSize);
						memcpy(&rowPtr[dstX], &rowPtr[srcX], elemSize);
						memcpy(&rowPtr[srcX], elemTemp, elemSize);
					}

					rowPtr += pixelData.getRowPitch() * elemSize;
				}

				slicePtr += pixelData.getSlicePitch() * elemSize;
			}

			bs_stack_free(elemTemp);
		}
	}

	void PixelUtil::applyGamma(UINT8* buffer, float gamma, UINT32 size, UINT8 bpp)
	{
		if(gamma == 1.0f)
			return;

		UINT32 stride = bpp >> 3;

		for(size_t i = 0, j = size / stride; i < j; i++, buffer += stride)
		{
			float r = (float)buffer[0];
			float g = (float)buffer[1];
			float b = (float)buffer[2];

			r = r * gamma;
			g = g * gamma;
			b = b * gamma;

			float scale = 1.0f;
			float tmp = 0.0f;

			if(r > 255.0f && (tmp=(255.0f/r)) < scale)
				scale = tmp;

			if(g > 255.0f && (tmp=(255.0f/g)) < scale)
				scale = tmp;

			if(b > 255.0f && (tmp=(255.0f/b)) < scale)
				scale = tmp;

			r *= scale;
			g *= scale;
			b *= scale;

			buffer[0] = (UINT8)r;
			buffer[1] = (UINT8)g;
			buffer[2] = (UINT8)b;
		}
	}

	void PixelUtil::compress(const PixelData& src, PixelData& dst, const CompressionOptions& options)
	{
		if (!isCompressed(options.format))
		{
			LOGERR("Compression failed. Destination format is not a valid compressed format.")
			return;
		}

		if (src.getDepth() != 1)
		{
			LOGERR("Compression failed. 3D texture compression not supported.")
			return;
		}

		if (isCompressed(src.getFormat()))
		{
			LOGERR("Compression failed. Source data cannot be compressed.");
			return;
		}

		PixelFormat interimFormat = options.format == PF_BC6H ? PF_RGBA32F : PF_BGRA8;

		PixelData interimData(src.getWidth(), src.getHeight(), 1, interimFormat);
		interimData.allocateInternalBuffer();
		bulkPixelConversion(src, interimData);

		nvtt::InputOptions io;
		io.setTextureLayout(nvtt::TextureType_2D, src.getWidth(), src.getHeight());
		io.setMipmapGeneration(false);
		io.setAlphaMode(toNVTTAlphaMode(options.alphaMode));
		io.setNormalMap(options.isNormalMap);

		if (interimFormat == PF_RGBA32F)
			io.setFormat(nvtt::InputFormat_RGBA_32F);
		else
			io.setFormat(nvtt::InputFormat_BGRA_8UB);

		if (options.isSRGB)
			io.setGamma(2.2f, 2.2f);
		else
			io.setGamma(1.0f, 1.0f);

		io.setMipmapData(interimData.getData(), src.getWidth(), src.getHeight());

		nvtt::CompressionOptions co;
		co.setFormat(toNVTTFormat(options.format));
		co.setQuality(toNVTTQuality(options.quality));

		NVTTCompressOutputHandler outputHandler(dst.getData(), dst.getConsecutiveSize());

		nvtt::OutputOptions oo;
		oo.setOutputHeader(false);
		oo.setOutputHandler(&outputHandler);

		nvtt::Compressor compressor;
		if (!compressor.process(io, co, oo))
		{
			LOGERR("Compression failed. Internal error.");
			return;
		}
	}

	Vector<SPtr<PixelData>> PixelUtil::genMipmaps(const PixelData& src, const MipMapGenOptions& options)
	{
		Vector<SPtr<PixelData>> outputMipBuffers;

		if (src.getDepth() != 1)
		{
			LOGERR("Mipmap generation failed. 3D texture formats not supported.")
			return outputMipBuffers;
		}

		if (isCompressed(src.getFormat()))
		{
			LOGERR("Mipmap generation failed. Source data cannot be compressed.")
			return outputMipBuffers;
		}

		if (!Bitwise::isPow2(src.getWidth()) || !Bitwise::isPow2(src.getHeight()))
		{
			LOGERR("Mipmap generation failed. Texture width & height must be powers of 2.");
			return outputMipBuffers;
		}

		PixelFormat interimFormat = isFloatingPoint(src.getFormat()) ? PF_RGBA32F : PF_BGRA8;

		PixelData interimData(src.getWidth(), src.getHeight(), 1, interimFormat);
		interimData.allocateInternalBuffer();
		bulkPixelConversion(src, interimData);

		if (interimFormat != PF_RGBA32F)
			flipComponentOrder(interimData);

		nvtt::InputOptions io;
		io.setTextureLayout(nvtt::TextureType_2D, src.getWidth(), src.getHeight());
		io.setMipmapGeneration(true);
		io.setNormalMap(options.isNormalMap);
		io.setNormalizeMipmaps(options.normalizeMipmaps);
		io.setWrapMode(toNVTTWrapMode(options.wrapMode));

		if (interimFormat == PF_RGBA32F)
			io.setFormat(nvtt::InputFormat_RGBA_32F);
		else
			io.setFormat(nvtt::InputFormat_BGRA_8UB);

		if (options.isSRGB)
			io.setGamma(2.2f, 2.2f);
		else
			io.setGamma(1.0f, 1.0f);

		io.setMipmapData(interimData.getData(), src.getWidth(), src.getHeight());

		nvtt::CompressionOptions co;
		co.setFormat(nvtt::Format_RGBA);

		if (interimFormat == PF_RGBA32F)
		{
			co.setPixelType(nvtt::PixelType_Float);
			co.setPixelFormat(32, 32, 32, 32);
		}
		else
		{
			co.setPixelType(nvtt::PixelType_UnsignedNorm);
			co.setPixelFormat(32, 0x0000FF00, 0x00FF0000, 0xFF000000, 0x000000FF);
		}

		UINT32 numMips = getMaxMipmaps(src.getWidth(), src.getHeight(), 1, src.getFormat());

		Vector<SPtr<PixelData>> rgbaMipBuffers;

		// Note: This can be done more effectively without creating so many temp buffers
		// and working with the original formats directly, but it would complicate the code
		// too much at the moment.
		UINT32 curWidth = src.getWidth();
		UINT32 curHeight = src.getHeight();
		for (UINT32 i = 0; i < numMips; i++)
		{
			rgbaMipBuffers.push_back(bs_shared_ptr_new<PixelData>(curWidth, curHeight, 1, interimFormat));
			rgbaMipBuffers.back()->allocateInternalBuffer();

			if (curWidth > 1)
				curWidth = curWidth / 2;

			if (curHeight > 1)
				curHeight = curHeight / 2;
		}

		rgbaMipBuffers.push_back(bs_shared_ptr_new<PixelData>(curWidth, curHeight, 1, interimFormat));
		rgbaMipBuffers.back()->allocateInternalBuffer();

		NVTTMipmapOutputHandler outputHandler(rgbaMipBuffers);

		nvtt::OutputOptions oo;
		oo.setOutputHeader(false);
		oo.setOutputHandler(&outputHandler);

		nvtt::Compressor compressor;
		if (!compressor.process(io, co, oo))
		{
			LOGERR("Mipmap generation failed. Internal error.");
			return outputMipBuffers;
		}

		interimData.freeInternalBuffer();

		for (UINT32 i = 0; i < (UINT32)rgbaMipBuffers.size(); i++)
		{
			SPtr<PixelData> argbBuffer = rgbaMipBuffers[i];
			SPtr<PixelData> outputBuffer = bs_shared_ptr_new<PixelData>(argbBuffer->getWidth(), argbBuffer->getHeight(), 1, src.getFormat());
			outputBuffer->allocateInternalBuffer();

			bulkPixelConversion(*argbBuffer, *outputBuffer);
			argbBuffer->freeInternalBuffer();

			outputMipBuffers.push_back(outputBuffer);
		}

		return outputMipBuffers;
	}
}

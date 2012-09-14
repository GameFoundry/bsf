/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#include "CmPixelUtil.h"
#include "CmBitwise.h"
#include "CmColor.h"
#include "CmException.h"


namespace CamelotEngine {

	//-----------------------------------------------------------------------
    /**
    * Resamplers
    */

	// variable name hints:
	// sx_48 = 16/48-bit fixed-point x-position in source
	// stepx = difference between adjacent sx_48 values
	// sx1 = lower-bound integer x-position in source
	// sx2 = upper-bound integer x-position in source
	// sxf = fractional weight beween sx1 and sx2
	// x,y,z = location of output pixel in destination

	// nearest-neighbor resampler, does not convert formats.
	// templated on bytes-per-pixel to allow compiler optimizations, such
	// as simplifying memcpy() and replacing multiplies with bitshifts
	template<unsigned int elemsize> struct NearestResampler {
		static void scale(const PixelData& src, const PixelData& dst) {
			// assert(src.format == dst.format);

			// srcdata stays at beginning, pdst is a moving pointer
			UINT8* srcdata = (UINT8*)src.data;
			UINT8* pdst = (UINT8*)dst.data;

			// sx_48,sy_48,sz_48 represent current position in source
			// using 16/48-bit fixed precision, incremented by steps
			UINT64 stepx = ((UINT64)src.getWidth() << 48) / dst.getWidth();
			UINT64 stepy = ((UINT64)src.getHeight() << 48) / dst.getHeight();
			UINT64 stepz = ((UINT64)src.getDepth() << 48) / dst.getDepth();

			// note: ((stepz>>1) - 1) is an extra half-step increment to adjust
			// for the center of the destination pixel, not the top-left corner
			UINT64 sz_48 = (stepz >> 1) - 1;
			for (size_t z = dst.front; z < dst.back; z++, sz_48 += stepz) {
				size_t srczoff = (size_t)(sz_48 >> 48) * src.slicePitch;

				UINT64 sy_48 = (stepy >> 1) - 1;
				for (size_t y = dst.top; y < dst.bottom; y++, sy_48 += stepy) {
					size_t srcyoff = (size_t)(sy_48 >> 48) * src.rowPitch;

					UINT64 sx_48 = (stepx >> 1) - 1;
					for (size_t x = dst.left; x < dst.right; x++, sx_48 += stepx) {
						UINT8* psrc = srcdata +
							elemsize*((size_t)(sx_48 >> 48) + srcyoff + srczoff);
						memcpy(pdst, psrc, elemsize);
						pdst += elemsize;
					}
					pdst += elemsize*dst.getRowSkip();
				}
				pdst += elemsize*dst.getSliceSkip();
			}
		}
	};


	// default floating-point linear resampler, does format conversion
	struct LinearResampler {
		static void scale(const PixelData& src, const PixelData& dst) {
			size_t srcelemsize = PixelUtil::getNumElemBytes(src.format);
			size_t dstelemsize = PixelUtil::getNumElemBytes(dst.format);

			// srcdata stays at beginning, pdst is a moving pointer
			UINT8* srcdata = (UINT8*)src.data;
			UINT8* pdst = (UINT8*)dst.data;

			// sx_48,sy_48,sz_48 represent current position in source
			// using 16/48-bit fixed precision, incremented by steps
			UINT64 stepx = ((UINT64)src.getWidth() << 48) / dst.getWidth();
			UINT64 stepy = ((UINT64)src.getHeight() << 48) / dst.getHeight();
			UINT64 stepz = ((UINT64)src.getDepth() << 48) / dst.getDepth();

			// temp is 16/16 bit fixed precision, used to adjust a source
			// coordinate (x, y, or z) backwards by half a pixel so that the
			// integer bits represent the first sample (eg, sx1) and the
			// fractional bits are the blend weight of the second sample
			unsigned int temp;

			// note: ((stepz>>1) - 1) is an extra half-step increment to adjust
			// for the center of the destination pixel, not the top-left corner
			UINT64 sz_48 = (stepz >> 1) - 1;
			for (size_t z = dst.front; z < dst.back; z++, sz_48+=stepz) {
				temp = static_cast<unsigned int>(sz_48 >> 32);
				temp = (temp > 0x8000)? temp - 0x8000 : 0;
				size_t sz1 = temp >> 16;				 // src z, sample #1
				size_t sz2 = std::min(sz1+1,src.getDepth()-1);// src z, sample #2
				float szf = (temp & 0xFFFF) / 65536.f; // weight of sample #2

				UINT64 sy_48 = (stepy >> 1) - 1;
				for (size_t y = dst.top; y < dst.bottom; y++, sy_48+=stepy) {
					temp = static_cast<unsigned int>(sy_48 >> 32);
					temp = (temp > 0x8000)? temp - 0x8000 : 0;
					size_t sy1 = temp >> 16;					// src y #1
					size_t sy2 = std::min(sy1+1,src.getHeight()-1);// src y #2
					float syf = (temp & 0xFFFF) / 65536.f; // weight of #2

					UINT64 sx_48 = (stepx >> 1) - 1;
					for (size_t x = dst.left; x < dst.right; x++, sx_48+=stepx) {
						temp = static_cast<unsigned int>(sx_48 >> 32);
						temp = (temp > 0x8000)? temp - 0x8000 : 0;
						size_t sx1 = temp >> 16;					// src x #1
						size_t sx2 = std::min(sx1+1,src.getWidth()-1);// src x #2
						float sxf = (temp & 0xFFFF) / 65536.f; // weight of #2

						Color x1y1z1, x2y1z1, x1y2z1, x2y2z1;
						Color x1y1z2, x2y1z2, x1y2z2, x2y2z2;

#define UNPACK(dst,x,y,z) PixelUtil::unpackColour(&dst, src.format, \
	srcdata + srcelemsize*((x)+(y)*src.rowPitch+(z)*src.slicePitch))

						UNPACK(x1y1z1,sx1,sy1,sz1); UNPACK(x2y1z1,sx2,sy1,sz1);
						UNPACK(x1y2z1,sx1,sy2,sz1); UNPACK(x2y2z1,sx2,sy2,sz1);
						UNPACK(x1y1z2,sx1,sy1,sz2); UNPACK(x2y1z2,sx2,sy1,sz2);
						UNPACK(x1y2z2,sx1,sy2,sz2); UNPACK(x2y2z2,sx2,sy2,sz2);
#undef UNPACK

						Color accum =
							x1y1z1 * ((1.0f - sxf)*(1.0f - syf)*(1.0f - szf)) +
							x2y1z1 * (        sxf *(1.0f - syf)*(1.0f - szf)) +
							x1y2z1 * ((1.0f - sxf)*        syf *(1.0f - szf)) +
							x2y2z1 * (        sxf *        syf *(1.0f - szf)) +
							x1y1z2 * ((1.0f - sxf)*(1.0f - syf)*        szf ) +
							x2y1z2 * (        sxf *(1.0f - syf)*        szf ) +
							x1y2z2 * ((1.0f - sxf)*        syf *        szf ) +
							x2y2z2 * (        sxf *        syf *        szf );

						PixelUtil::packColour(accum, dst.format, pdst);

						pdst += dstelemsize;
					}
					pdst += dstelemsize*dst.getRowSkip();
				}
				pdst += dstelemsize*dst.getSliceSkip();
			}
		}
	};


	// float32 linear resampler, converts FLOAT32_RGB/FLOAT32_RGBA only.
	// avoids overhead of pixel unpack/repack function calls
	struct LinearResampler_Float32 {
		static void scale(const PixelData& src, const PixelData& dst) {
			size_t srcchannels = PixelUtil::getNumElemBytes(src.format) / sizeof(float);
			size_t dstchannels = PixelUtil::getNumElemBytes(dst.format) / sizeof(float);
			// assert(srcchannels == 3 || srcchannels == 4);
			// assert(dstchannels == 3 || dstchannels == 4);

			// srcdata stays at beginning, pdst is a moving pointer
			float* srcdata = (float*)src.data;
			float* pdst = (float*)dst.data;

			// sx_48,sy_48,sz_48 represent current position in source
			// using 16/48-bit fixed precision, incremented by steps
			UINT64 stepx = ((UINT64)src.getWidth() << 48) / dst.getWidth();
			UINT64 stepy = ((UINT64)src.getHeight() << 48) / dst.getHeight();
			UINT64 stepz = ((UINT64)src.getDepth() << 48) / dst.getDepth();

			// temp is 16/16 bit fixed precision, used to adjust a source
			// coordinate (x, y, or z) backwards by half a pixel so that the
			// integer bits represent the first sample (eg, sx1) and the
			// fractional bits are the blend weight of the second sample
			unsigned int temp;

			// note: ((stepz>>1) - 1) is an extra half-step increment to adjust
			// for the center of the destination pixel, not the top-left corner
			UINT64 sz_48 = (stepz >> 1) - 1;
			for (size_t z = dst.front; z < dst.back; z++, sz_48+=stepz) {
				temp = static_cast<unsigned int>(sz_48 >> 32);
				temp = (temp > 0x8000)? temp - 0x8000 : 0;
				size_t sz1 = temp >> 16;				 // src z, sample #1
				size_t sz2 = std::min(sz1+1,src.getDepth()-1);// src z, sample #2
				float szf = (temp & 0xFFFF) / 65536.f; // weight of sample #2

				UINT64 sy_48 = (stepy >> 1) - 1;
				for (size_t y = dst.top; y < dst.bottom; y++, sy_48+=stepy) {
					temp = static_cast<unsigned int>(sy_48 >> 32);
					temp = (temp > 0x8000)? temp - 0x8000 : 0;
					size_t sy1 = temp >> 16;					// src y #1
					size_t sy2 = std::min(sy1+1,src.getHeight()-1);// src y #2
					float syf = (temp & 0xFFFF) / 65536.f; // weight of #2

					UINT64 sx_48 = (stepx >> 1) - 1;
					for (size_t x = dst.left; x < dst.right; x++, sx_48+=stepx) {
						temp = static_cast<unsigned int>(sx_48 >> 32);
						temp = (temp > 0x8000)? temp - 0x8000 : 0;
						size_t sx1 = temp >> 16;					// src x #1
						size_t sx2 = std::min(sx1+1,src.getWidth()-1);// src x #2
						float sxf = (temp & 0xFFFF) / 65536.f; // weight of #2

						// process R,G,B,A simultaneously for cache coherence?
						float accum[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

#define ACCUM3(x,y,z,factor) \
						{ float f = factor; \
						size_t off = (x+y*src.rowPitch+z*src.slicePitch)*srcchannels; \
						accum[0]+=srcdata[off+0]*f; accum[1]+=srcdata[off+1]*f; \
						accum[2]+=srcdata[off+2]*f; }

#define ACCUM4(x,y,z,factor) \
						{ float f = factor; \
						size_t off = (x+y*src.rowPitch+z*src.slicePitch)*srcchannels; \
						accum[0]+=srcdata[off+0]*f; accum[1]+=srcdata[off+1]*f; \
						accum[2]+=srcdata[off+2]*f; accum[3]+=srcdata[off+3]*f; }

						if (srcchannels == 3 || dstchannels == 3) {
							// RGB, no alpha
							ACCUM3(sx1,sy1,sz1,(1.0f-sxf)*(1.0f-syf)*(1.0f-szf));
							ACCUM3(sx2,sy1,sz1,      sxf *(1.0f-syf)*(1.0f-szf));
							ACCUM3(sx1,sy2,sz1,(1.0f-sxf)*      syf *(1.0f-szf));
							ACCUM3(sx2,sy2,sz1,      sxf *      syf *(1.0f-szf));
							ACCUM3(sx1,sy1,sz2,(1.0f-sxf)*(1.0f-syf)*      szf );
							ACCUM3(sx2,sy1,sz2,      sxf *(1.0f-syf)*      szf );
							ACCUM3(sx1,sy2,sz2,(1.0f-sxf)*      syf *      szf );
							ACCUM3(sx2,sy2,sz2,      sxf *      syf *      szf );
							accum[3] = 1.0f;
						} else {
							// RGBA
							ACCUM4(sx1,sy1,sz1,(1.0f-sxf)*(1.0f-syf)*(1.0f-szf));
							ACCUM4(sx2,sy1,sz1,      sxf *(1.0f-syf)*(1.0f-szf));
							ACCUM4(sx1,sy2,sz1,(1.0f-sxf)*      syf *(1.0f-szf));
							ACCUM4(sx2,sy2,sz1,      sxf *      syf *(1.0f-szf));
							ACCUM4(sx1,sy1,sz2,(1.0f-sxf)*(1.0f-syf)*      szf );
							ACCUM4(sx2,sy1,sz2,      sxf *(1.0f-syf)*      szf );
							ACCUM4(sx1,sy2,sz2,(1.0f-sxf)*      syf *      szf );
							ACCUM4(sx2,sy2,sz2,      sxf *      syf *      szf );
						}

						memcpy(pdst, accum, sizeof(float)*dstchannels);

#undef ACCUM3
#undef ACCUM4

						pdst += dstchannels;
					}
					pdst += dstchannels*dst.getRowSkip();
				}
				pdst += dstchannels*dst.getSliceSkip();
			}
		}
	};



	// byte linear resampler, does not do any format conversions.
	// only handles pixel formats that use 1 byte per color channel.
	// 2D only; punts 3D pixelboxes to default LinearResampler (slow).
	// templated on bytes-per-pixel to allow compiler optimizations, such
	// as unrolling loops and replacing multiplies with bitshifts
	template<unsigned int channels> struct LinearResampler_Byte {
		static void scale(const PixelData& src, const PixelData& dst) {
			// assert(src.format == dst.format);

			// only optimized for 2D
			if (src.getDepth() > 1 || dst.getDepth() > 1) {
				LinearResampler::scale(src, dst);
				return;
			}

			// srcdata stays at beginning of slice, pdst is a moving pointer
			UINT8* srcdata = (UINT8*)src.data;
			UINT8* pdst = (UINT8*)dst.data;

			// sx_48,sy_48 represent current position in source
			// using 16/48-bit fixed precision, incremented by steps
			UINT64 stepx = ((UINT64)src.getWidth() << 48) / dst.getWidth();
			UINT64 stepy = ((UINT64)src.getHeight() << 48) / dst.getHeight();

			// bottom 28 bits of temp are 16/12 bit fixed precision, used to
			// adjust a source coordinate backwards by half a pixel so that the
			// integer bits represent the first sample (eg, sx1) and the
			// fractional bits are the blend weight of the second sample
			unsigned int temp;

			UINT64 sy_48 = (stepy >> 1) - 1;
			for (size_t y = dst.top; y < dst.bottom; y++, sy_48+=stepy) {
				temp = static_cast<unsigned int>(sy_48 >> 36);
				temp = (temp > 0x800)? temp - 0x800: 0;
				unsigned int syf = temp & 0xFFF;
				size_t sy1 = temp >> 12;
				size_t sy2 = std::min(sy1+1, src.bottom-src.top-1);
				size_t syoff1 = sy1 * src.rowPitch;
				size_t syoff2 = sy2 * src.rowPitch;

				UINT64 sx_48 = (stepx >> 1) - 1;
				for (size_t x = dst.left; x < dst.right; x++, sx_48+=stepx) {
					temp = static_cast<unsigned int>(sx_48 >> 36);
					temp = (temp > 0x800)? temp - 0x800 : 0;
					unsigned int sxf = temp & 0xFFF;
					size_t sx1 = temp >> 12;
					size_t sx2 = std::min(sx1+1, src.right-src.left-1);

					unsigned int sxfsyf = sxf*syf;
					for (unsigned int k = 0; k < channels; k++) {
						unsigned int accum =
							srcdata[(sx1 + syoff1)*channels+k]*(0x1000000-(sxf<<12)-(syf<<12)+sxfsyf) +
							srcdata[(sx2 + syoff1)*channels+k]*((sxf<<12)-sxfsyf) +
							srcdata[(sx1 + syoff2)*channels+k]*((syf<<12)-sxfsyf) +
							srcdata[(sx2 + syoff2)*channels+k]*sxfsyf;
						// accum is computed using 8/24-bit fixed-point math
						// (maximum is 0xFF000000; rounding will not cause overflow)
						*pdst++ = static_cast<UINT8>((accum + 0x800000) >> 24);
					}
				}
				pdst += channels*dst.getRowSkip();
			}
		}
	};

    //-----------------------------------------------------------------------
    /**
    * A record that describes a pixel format in detail.
    */
    struct PixelFormatDescription {
        /* Name of the format, as in the enum */
        const char *name;
        /* Number of bytes one element (colour value) takes. */
        unsigned char elemBytes;
        /* Pixel format flags, see enum PixelFormatFlags for the bit field
        * definitions
        */
        UINT32 flags;
        /** Component type
         */
        PixelComponentType componentType;
        /** Component count
         */
        unsigned char componentCount;
        /* Number of bits for red(or luminance), green, blue, alpha
        */
        unsigned char rbits,gbits,bbits,abits; /*, ibits, dbits, ... */

        /* Masks and shifts as used by packers/unpackers */
        UINT32 rmask, gmask, bmask, amask;
        unsigned char rshift, gshift, bshift, ashift;
    };
    //-----------------------------------------------------------------------
    /** Pixel format database */
    PixelFormatDescription _pixelFormats[PF_COUNT] = {
	//-----------------------------------------------------------------------
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
        0, 0, 0, 0, 0, 0, 0, 0
        },
	//-----------------------------------------------------------------------
        {"PF_L8",
        /* Bytes per element */
        1,
        /* Flags */
        PFF_LUMINANCE | PFF_NATIVEENDIAN,
        /* Component type and count */
        PCT_BYTE, 1,
        /* rbits, gbits, bbits, abits */
        8, 0, 0, 0,
        /* Masks and shifts */
        0xFF, 0, 0, 0, 0, 0, 0, 0
        },
	//-----------------------------------------------------------------------
        {"PF_L16",
        /* Bytes per element */
        2,
        /* Flags */
        PFF_LUMINANCE | PFF_NATIVEENDIAN,
        /* Component type and count */
        PCT_SHORT, 1,
        /* rbits, gbits, bbits, abits */
        16, 0, 0, 0,
        /* Masks and shifts */
        0xFFFF, 0, 0, 0, 0, 0, 0, 0
        },
	//-----------------------------------------------------------------------
        {"PF_A8",
        /* Bytes per element */
        1,
        /* Flags */
        PFF_HASALPHA | PFF_NATIVEENDIAN,
        /* Component type and count */
        PCT_BYTE, 1,
        /* rbits, gbits, bbits, abits */
        0, 0, 0, 8,
        /* Masks and shifts */
        0, 0, 0, 0xFF, 0, 0, 0, 0
        },
	//-----------------------------------------------------------------------
        {"PF_A4L4",
        /* Bytes per element */
        1,
        /* Flags */
        PFF_HASALPHA | PFF_LUMINANCE | PFF_NATIVEENDIAN,
        /* Component type and count */
        PCT_BYTE, 2,
        /* rbits, gbits, bbits, abits */
        4, 0, 0, 4,
        /* Masks and shifts */
        0x0F, 0, 0, 0xF0, 0, 0, 0, 4
        },
	//-----------------------------------------------------------------------
        {"PF_BYTE_LA",
        /* Bytes per element */
        2,
        /* Flags */
        PFF_HASALPHA | PFF_LUMINANCE,
        /* Component type and count */
        PCT_BYTE, 2,
        /* rbits, gbits, bbits, abits */
        8, 0, 0, 8,
        /* Masks and shifts */
        0,0,0,0,0,0,0,0
        },
	//-----------------------------------------------------------------------
        {"PF_R5G6B5",
        /* Bytes per element */
        2,
        /* Flags */
        PFF_NATIVEENDIAN,
        /* Component type and count */
        PCT_BYTE, 3,
        /* rbits, gbits, bbits, abits */
        5, 6, 5, 0,
        /* Masks and shifts */
        0xF800, 0x07E0, 0x001F, 0,
        11, 5, 0, 0
        },
	//-----------------------------------------------------------------------
		{"PF_B5G6R5",
        /* Bytes per element */
        2,
        /* Flags */
        PFF_NATIVEENDIAN,
        /* Component type and count */
        PCT_BYTE, 3,
        /* rbits, gbits, bbits, abits */
        5, 6, 5, 0,
        /* Masks and shifts */
        0x001F, 0x07E0, 0xF800, 0,
        0, 5, 11, 0
        },
	//-----------------------------------------------------------------------
        {"PF_A4R4G4B4",
        /* Bytes per element */
        2,
        /* Flags */
        PFF_HASALPHA | PFF_NATIVEENDIAN,
        /* Component type and count */
        PCT_BYTE, 4,
        /* rbits, gbits, bbits, abits */
        4, 4, 4, 4,
        /* Masks and shifts */
        0x0F00, 0x00F0, 0x000F, 0xF000,
        8, 4, 0, 12
        },
	//-----------------------------------------------------------------------
        {"PF_A1R5G5B5",
        /* Bytes per element */
        2,
        /* Flags */
        PFF_HASALPHA | PFF_NATIVEENDIAN,
        /* Component type and count */
        PCT_BYTE, 4,
        /* rbits, gbits, bbits, abits */
        5, 5, 5, 1,
        /* Masks and shifts */
        0x7C00, 0x03E0, 0x001F, 0x8000,
        10, 5, 0, 15,
        },
	//-----------------------------------------------------------------------
        {"PF_R8G8B8",
        /* Bytes per element */
        3,  // 24 bit integer -- special
        /* Flags */
        PFF_NATIVEENDIAN,
        /* Component type and count */
        PCT_BYTE, 3,
        /* rbits, gbits, bbits, abits */
        8, 8, 8, 0,
        /* Masks and shifts */
        0xFF0000, 0x00FF00, 0x0000FF, 0,
        16, 8, 0, 0
        },
	//-----------------------------------------------------------------------
        {"PF_B8G8R8",
        /* Bytes per element */
        3,  // 24 bit integer -- special
        /* Flags */
        PFF_NATIVEENDIAN,
        /* Component type and count */
        PCT_BYTE, 3,
        /* rbits, gbits, bbits, abits */
        8, 8, 8, 0,
        /* Masks and shifts */
        0x0000FF, 0x00FF00, 0xFF0000, 0,
        0, 8, 16, 0
        },
	//-----------------------------------------------------------------------
        {"PF_A8R8G8B8",
        /* Bytes per element */
        4,
        /* Flags */
        PFF_HASALPHA | PFF_NATIVEENDIAN,
        /* Component type and count */
        PCT_BYTE, 4,
        /* rbits, gbits, bbits, abits */
        8, 8, 8, 8,
        /* Masks and shifts */
        0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000,
        16, 8, 0, 24
        },
	//-----------------------------------------------------------------------
        {"PF_A8B8G8R8",
        /* Bytes per element */
        4,
        /* Flags */
        PFF_HASALPHA | PFF_NATIVEENDIAN,
        /* Component type and count */
        PCT_BYTE, 4,
        /* rbits, gbits, bbits, abits */
        8, 8, 8, 8,
        /* Masks and shifts */
        0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000,
        0, 8, 16, 24,
        },
	//-----------------------------------------------------------------------
        {"PF_B8G8R8A8",
        /* Bytes per element */
        4,
        /* Flags */
        PFF_HASALPHA | PFF_NATIVEENDIAN,
        /* Component type and count */
        PCT_BYTE, 4,
        /* rbits, gbits, bbits, abits */
        8, 8, 8, 8,
        /* Masks and shifts */
        0x0000FF00, 0x00FF0000, 0xFF000000, 0x000000FF,
        8, 16, 24, 0
        },
	//-----------------------------------------------------------------------
        {"PF_A2R10G10B10",
        /* Bytes per element */
        4,
        /* Flags */
        PFF_HASALPHA | PFF_NATIVEENDIAN,
        /* Component type and count */
        PCT_BYTE, 4,
        /* rbits, gbits, bbits, abits */
        10, 10, 10, 2,
        /* Masks and shifts */
        0x3FF00000, 0x000FFC00, 0x000003FF, 0xC0000000,
        20, 10, 0, 30
        },
	//-----------------------------------------------------------------------
        {"PF_A2B10G10R10",
        /* Bytes per element */
        4,
        /* Flags */
        PFF_HASALPHA | PFF_NATIVEENDIAN,
        /* Component type and count */
        PCT_BYTE, 4,
        /* rbits, gbits, bbits, abits */
        10, 10, 10, 2,
        /* Masks and shifts */
        0x000003FF, 0x000FFC00, 0x3FF00000, 0xC0000000,
        0, 10, 20, 30
        },
	//-----------------------------------------------------------------------
        {"PF_DXT1",
        /* Bytes per element */
        0,
        /* Flags */
        PFF_COMPRESSED | PFF_HASALPHA,
        /* Component type and count */
        PCT_BYTE, 3, // No alpha
        /* rbits, gbits, bbits, abits */
        0, 0, 0, 0,
        /* Masks and shifts */
        0, 0, 0, 0, 0, 0, 0, 0
        },
	//-----------------------------------------------------------------------
        {"PF_DXT2",
        /* Bytes per element */
        0,
        /* Flags */
        PFF_COMPRESSED | PFF_HASALPHA,
        /* Component type and count */
        PCT_BYTE, 4,
        /* rbits, gbits, bbits, abits */
        0, 0, 0, 0,
        /* Masks and shifts */
        0, 0, 0, 0, 0, 0, 0, 0
        },
	//-----------------------------------------------------------------------
        {"PF_DXT3",
        /* Bytes per element */
        0,
        /* Flags */
        PFF_COMPRESSED | PFF_HASALPHA,
        /* Component type and count */
        PCT_BYTE, 4,
        /* rbits, gbits, bbits, abits */
        0, 0, 0, 0,
        /* Masks and shifts */
        0, 0, 0, 0, 0, 0, 0, 0
        },
	//-----------------------------------------------------------------------
        {"PF_DXT4",
        /* Bytes per element */
        0,
        /* Flags */
        PFF_COMPRESSED | PFF_HASALPHA,
        /* Component type and count */
        PCT_BYTE, 4,
        /* rbits, gbits, bbits, abits */
        0, 0, 0, 0,
        /* Masks and shifts */
        0, 0, 0, 0, 0, 0, 0, 0
        },
	//-----------------------------------------------------------------------
        {"PF_DXT5",
        /* Bytes per element */
        0,
        /* Flags */
        PFF_COMPRESSED | PFF_HASALPHA,
        /* Component type and count */
        PCT_BYTE, 4,
        /* rbits, gbits, bbits, abits */
        0, 0, 0, 0,
        /* Masks and shifts */
        0, 0, 0, 0, 0, 0, 0, 0
        },
	//-----------------------------------------------------------------------
        {"PF_FLOAT16_RGB",
        /* Bytes per element */
        6,
        /* Flags */
        PFF_FLOAT,
        /* Component type and count */
        PCT_FLOAT16, 3,
        /* rbits, gbits, bbits, abits */
        16, 16, 16, 0,
        /* Masks and shifts */
        0, 0, 0, 0, 0, 0, 0, 0
        },
	//-----------------------------------------------------------------------
        {"PF_FLOAT16_RGBA",
        /* Bytes per element */
        8,
        /* Flags */
        PFF_FLOAT | PFF_HASALPHA,
        /* Component type and count */
        PCT_FLOAT16, 4,
        /* rbits, gbits, bbits, abits */
        16, 16, 16, 16,
        /* Masks and shifts */
        0, 0, 0, 0, 0, 0, 0, 0
        },
	//-----------------------------------------------------------------------
        {"PF_FLOAT32_RGB",
        /* Bytes per element */
        12,
        /* Flags */
        PFF_FLOAT,
        /* Component type and count */
        PCT_FLOAT32, 3,
        /* rbits, gbits, bbits, abits */
        32, 32, 32, 0,
        /* Masks and shifts */
        0, 0, 0, 0, 0, 0, 0, 0
        },
	//-----------------------------------------------------------------------
        {"PF_FLOAT32_RGBA",
        /* Bytes per element */
        16,
        /* Flags */
        PFF_FLOAT | PFF_HASALPHA,
        /* Component type and count */
        PCT_FLOAT32, 4,
        /* rbits, gbits, bbits, abits */
        32, 32, 32, 32,
        /* Masks and shifts */
        0, 0, 0, 0, 0, 0, 0, 0
        },
	//-----------------------------------------------------------------------
        {"PF_X8R8G8B8",
        /* Bytes per element */
        4,
        /* Flags */
        PFF_NATIVEENDIAN,
        /* Component type and count */
        PCT_BYTE, 3,
        /* rbits, gbits, bbits, abits */
        8, 8, 8, 0,
        /* Masks and shifts */
        0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000,
        16, 8, 0, 24
        },
	//-----------------------------------------------------------------------
        {"PF_X8B8G8R8",
        /* Bytes per element */
        4,
        /* Flags */
        PFF_NATIVEENDIAN,
        /* Component type and count */
        PCT_BYTE, 3,
        /* rbits, gbits, bbits, abits */
        8, 8, 8, 0,
        /* Masks and shifts */
        0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000,
        0, 8, 16, 24
        },
	//-----------------------------------------------------------------------
        {"PF_R8G8B8A8",
        /* Bytes per element */
        4,
        /* Flags */
        PFF_HASALPHA | PFF_NATIVEENDIAN,
        /* Component type and count */
        PCT_BYTE, 4,
        /* rbits, gbits, bbits, abits */
        8, 8, 8, 8,
        /* Masks and shifts */
        0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF,
        24, 16, 8, 0
        },
	//-----------------------------------------------------------------------
		{"PF_DEPTH",
        /* Bytes per element */
        4,
        /* Flags */
        PFF_DEPTH,
        /* Component type and count */
        PCT_FLOAT32, 1, // ?
        /* rbits, gbits, bbits, abits */
        0, 0, 0, 0,
        /* Masks and shifts */
		0, 0, 0, 0, 0, 0, 0, 0
        },
	//-----------------------------------------------------------------------
		{"PF_SHORT_RGBA",
		/* Bytes per element */
        8,
        /* Flags */
        PFF_HASALPHA,
        /* Component type and count */
        PCT_SHORT, 4,
        /* rbits, gbits, bbits, abits */
        16, 16, 16, 16,
        /* Masks and shifts */
		0, 0, 0, 0, 0, 0, 0, 0
        },
	//-----------------------------------------------------------------------
        {"PF_R3G3B2",
        /* Bytes per element */
        1,
        /* Flags */
        PFF_NATIVEENDIAN,
        /* Component type and count */
        PCT_BYTE, 3,
        /* rbits, gbits, bbits, abits */
        3, 3, 2, 0,
        /* Masks and shifts */
        0xE0, 0x1C, 0x03, 0,
        5, 2, 0, 0
        },
	//-----------------------------------------------------------------------
        {"PF_FLOAT16_R",
        /* Bytes per element */
        2,
        /* Flags */
        PFF_FLOAT,
        /* Component type and count */
        PCT_FLOAT16, 1,
        /* rbits, gbits, bbits, abits */
        16, 0, 0, 0,
        /* Masks and shifts */
        0, 0, 0, 0, 0, 0, 0, 0
        },
	//-----------------------------------------------------------------------
        {"PF_FLOAT32_R",
        /* Bytes per element */
        4,
        /* Flags */
        PFF_FLOAT,
        /* Component type and count */
        PCT_FLOAT32, 1,
        /* rbits, gbits, bbits, abits */
        32, 0, 0, 0,
        /* Masks and shifts */
        0, 0, 0, 0, 0, 0, 0, 0
        },
	//-----------------------------------------------------------------------
        {"PF_SHORT_GR",
        /* Bytes per element */
        4,
        /* Flags */
        PFF_NATIVEENDIAN,
        /* Component type and count */
        PCT_SHORT, 2,
        /* rbits, gbits, bbits, abits */
        16, 16, 0, 0,
        /* Masks and shifts */
        0x0000FFFF, 0xFFFF0000, 0, 0, 
		0, 16, 0, 0
        },
	//-----------------------------------------------------------------------
        {"PF_FLOAT16_GR",
        /* Bytes per element */
        4,
        /* Flags */
        PFF_FLOAT,
        /* Component type and count */
        PCT_FLOAT16, 2,
        /* rbits, gbits, bbits, abits */
        16, 16, 0, 0,
        /* Masks and shifts */
        0, 0, 0, 0, 0, 0, 0, 0
        },
	//-----------------------------------------------------------------------
        {"PF_FLOAT32_GR",
        /* Bytes per element */
        8,
        /* Flags */
        PFF_FLOAT,
        /* Component type and count */
        PCT_FLOAT32, 2,
        /* rbits, gbits, bbits, abits */
        32, 32, 0, 0,
        /* Masks and shifts */
        0, 0, 0, 0, 0, 0, 0, 0
        },
	//-----------------------------------------------------------------------
		{"PF_SHORT_RGB",
		/* Bytes per element */
        6,
        /* Flags */
        0,
        /* Component type and count */
        PCT_SHORT, 3,
        /* rbits, gbits, bbits, abits */
        16, 16, 16, 0,
        /* Masks and shifts */
		0, 0, 0, 0, 0, 0, 0, 0
        },
    //-----------------------------------------------------------------------
		{"PF_PVRTC_RGB2",
        /* Bytes per element */
        0,
        /* Flags */
        PFF_COMPRESSED,
        /* Component type and count */
        PCT_BYTE, 3,
        /* rbits, gbits, bbits, abits */
        0, 0, 0, 0,
        /* Masks and shifts */
        0, 0, 0, 0, 0, 0, 0, 0
        },
    //-----------------------------------------------------------------------
		{"PF_PVRTC_RGBA2",
        /* Bytes per element */
        0,
        /* Flags */
        PFF_COMPRESSED | PFF_HASALPHA,
        /* Component type and count */
        PCT_BYTE, 4,
        /* rbits, gbits, bbits, abits */
        0, 0, 0, 0,
        /* Masks and shifts */
        0, 0, 0, 0, 0, 0, 0, 0
        },
    //-----------------------------------------------------------------------
		{"PF_PVRTC_RGB4",
        /* Bytes per element */
        0,
        /* Flags */
        PFF_COMPRESSED,
        /* Component type and count */
        PCT_BYTE, 3,
        /* rbits, gbits, bbits, abits */
        0, 0, 0, 0,
        /* Masks and shifts */
        0, 0, 0, 0, 0, 0, 0, 0
        },
    //-----------------------------------------------------------------------
		{"PF_PVRTC_RGBA4",
        /* Bytes per element */
        0,
        /* Flags */
        PFF_COMPRESSED | PFF_HASALPHA,
        /* Component type and count */
        PCT_BYTE, 4,
        /* rbits, gbits, bbits, abits */
        0, 0, 0, 0,
        /* Masks and shifts */
        0, 0, 0, 0, 0, 0, 0, 0
        },
        
    };
    //-----------------------------------------------------------------------
	size_t PixelData::getConsecutiveSize() const
	{
		return PixelUtil::getMemorySize(getWidth(), getHeight(), getDepth(), format);
	}
	PixelData PixelData::getSubVolume(const Box &def) const
	{
		if(PixelUtil::isCompressed(format))
		{
			if(def.left == left && def.top == top && def.front == front &&
			   def.right == right && def.bottom == bottom && def.back == back)
			{
				// Entire buffer is being queried
				return *this;
			}
			CM_EXCEPT(InvalidParametersException, "Cannot return subvolume of compressed PixelBuffer");
		}
		if(!contains(def))
		{
			CM_EXCEPT(InvalidParametersException, "Bounds out of range");
		}

		const size_t elemSize = PixelUtil::getNumElemBytes(format);
		// Calculate new data origin
		// Notice how we do not propagate left/top/front from the incoming box, since
		// the returned pointer is already offset
		PixelData rval(def.getWidth(), def.getHeight(), def.getDepth(), format, 
			((UINT8*)data) + ((def.left-left)*elemSize)
			+ ((def.top-top)*rowPitch*elemSize)
			+ ((def.front-front)*slicePitch*elemSize)
		);

		rval.rowPitch = rowPitch;
		rval.slicePitch = slicePitch;
		rval.format = format;

		return rval;
	}
    //-----------------------------------------------------------------------
    /**
    * Directly get the description record for provided pixel format. For debug builds,
    * this checks the bounds of fmt with an assertion.
    */
    static inline const PixelFormatDescription &getDescriptionFor(const PixelFormat fmt)
    {
        const int ord = (int)fmt;
        assert(ord>=0 && ord<PF_COUNT);

        return _pixelFormats[ord];
    }
    //-----------------------------------------------------------------------
    size_t PixelUtil::getNumElemBytes( PixelFormat format )
    {
        return getDescriptionFor(format).elemBytes;
    }
	//-----------------------------------------------------------------------
	size_t PixelUtil::getMemorySize(size_t width, size_t height, size_t depth, PixelFormat format)
	{
		if(isCompressed(format))
		{
			switch(format)
			{
				// DXT formats work by dividing the image into 4x4 blocks, then encoding each
				// 4x4 block with a certain number of bytes. 
				case PF_DXT1:
					return ((width+3)/4)*((height+3)/4)*8 * depth;
				case PF_DXT2:
				case PF_DXT3:
				case PF_DXT4:
				case PF_DXT5:
					return ((width+3)/4)*((height+3)/4)*16 * depth;

                // Size calculations from the PVRTC OpenGL extension spec
                // http://www.khronos.org/registry/gles/extensions/IMG/IMG_texture_compression_pvrtc.txt
                // Basically, 32 bytes is the minimum texture size.  Smaller textures are padded up to 32 bytes
                case PF_PVRTC_RGB2:
                case PF_PVRTC_RGBA2:
					assert(depth == 1);
                    return (std::max((int)width, 16) * std::max((int)height, 8) * 2 + 7) / 8;
                case PF_PVRTC_RGB4:
                case PF_PVRTC_RGBA4:
					assert(depth == 1);
                    return (std::max((int)width, 8) * std::max((int)height, 8) * 4 + 7) / 8;
				default:
				CM_EXCEPT(InvalidParametersException, "Invalid compressed pixel format");
			}
		}
		else
		{
			return width*height*depth*getNumElemBytes(format);
		}
	}
    //-----------------------------------------------------------------------
    size_t PixelUtil::getNumElemBits( PixelFormat format )
    {
        return getDescriptionFor(format).elemBytes * 8;
    }
    //-----------------------------------------------------------------------
    unsigned int PixelUtil::getFlags( PixelFormat format )
    {
        return getDescriptionFor(format).flags;
    }
    //-----------------------------------------------------------------------
    bool PixelUtil::hasAlpha(PixelFormat format)
    {
        return (PixelUtil::getFlags(format) & PFF_HASALPHA) > 0;
    }
    //-----------------------------------------------------------------------
    bool PixelUtil::isFloatingPoint(PixelFormat format)
    {
        return (PixelUtil::getFlags(format) & PFF_FLOAT) > 0;
    }
    //-----------------------------------------------------------------------
    bool PixelUtil::isCompressed(PixelFormat format)
    {
        return (PixelUtil::getFlags(format) & PFF_COMPRESSED) > 0;
    }
    //-----------------------------------------------------------------------
    bool PixelUtil::isDepth(PixelFormat format)
    {
        return (PixelUtil::getFlags(format) & PFF_DEPTH) > 0;
    }
    //-----------------------------------------------------------------------
    bool PixelUtil::isNativeEndian(PixelFormat format)
    {
        return (PixelUtil::getFlags(format) & PFF_NATIVEENDIAN) > 0;
    }
    //-----------------------------------------------------------------------
    bool PixelUtil::isLuminance(PixelFormat format)
    {
        return (PixelUtil::getFlags(format) & PFF_LUMINANCE) > 0;
    }
    //-----------------------------------------------------------------------
	bool PixelUtil::isValidExtent(size_t width, size_t height, size_t depth, PixelFormat format)
	{
		if(isCompressed(format))
		{
			switch(format)
			{
				case PF_DXT1:
				case PF_DXT2:
				case PF_DXT3:
				case PF_DXT4:
				case PF_DXT5:
					return ((width&3)==0 && (height&3)==0 && depth==1);
				default:
					return true;
			}
		}
		else
		{
			return true;
		}
	}
	//-----------------------------------------------------------------------
    void PixelUtil::getBitDepths(PixelFormat format, int rgba[4])
    {
        const PixelFormatDescription &des = getDescriptionFor(format);
        rgba[0] = des.rbits;
        rgba[1] = des.gbits;
        rgba[2] = des.bbits;
        rgba[3] = des.abits;
    }
	//-----------------------------------------------------------------------
	void PixelUtil::getBitMasks(PixelFormat format, UINT32 rgba[4])
    {
        const PixelFormatDescription &des = getDescriptionFor(format);
        rgba[0] = des.rmask;
        rgba[1] = des.gmask;
        rgba[2] = des.bmask;
        rgba[3] = des.amask;
    }
	//---------------------------------------------------------------------
	void PixelUtil::getBitShifts(PixelFormat format, unsigned char rgba[4])
	{
		const PixelFormatDescription &des = getDescriptionFor(format);
		rgba[0] = des.rshift;
		rgba[1] = des.gshift;
		rgba[2] = des.bshift;
		rgba[3] = des.ashift;
	}
    //-----------------------------------------------------------------------
    String PixelUtil::getFormatName(PixelFormat srcformat)
    {
        return getDescriptionFor(srcformat).name;
    }
    //-----------------------------------------------------------------------
    bool PixelUtil::isAccessible(PixelFormat srcformat)
    {
        if (srcformat == PF_UNKNOWN)
            return false;
        unsigned int flags = getFlags(srcformat);
        return !((flags & PFF_COMPRESSED) || (flags & PFF_DEPTH));
    }
    //-----------------------------------------------------------------------
    PixelComponentType PixelUtil::getComponentType(PixelFormat fmt)
    {
        const PixelFormatDescription &des = getDescriptionFor(fmt);
        return des.componentType;
    }
    //-----------------------------------------------------------------------
    size_t PixelUtil::getComponentCount(PixelFormat fmt)
    {
        const PixelFormatDescription &des = getDescriptionFor(fmt);
        return des.componentCount;
    }
    //-----------------------------------------------------------------------
    PixelFormat PixelUtil::getFormatFromName(const String& name, bool accessibleOnly, bool caseSensitive)
    {
        String tmp = name;
        if (!caseSensitive)
        {
            // We are stored upper-case format names.
            StringUtil::toUpperCase(tmp);
        }

        for (int i = 0; i < PF_COUNT; ++i)
        {
            PixelFormat pf = static_cast<PixelFormat>(i);
            if (!accessibleOnly || isAccessible(pf))
            {
                if (tmp == getFormatName(pf))
                    return pf;
            }
        }
        return PF_UNKNOWN;
    }
    //-----------------------------------------------------------------------
    String PixelUtil::getBNFExpressionOfPixelFormats(bool accessibleOnly)
    {
        // Collect format names sorted by length, it's required by BNF compiler
        // that similar tokens need longer ones comes first.
        typedef multimap<String::size_type, String>::type FormatNameMap;
        FormatNameMap formatNames;
        for (size_t i = 0; i < PF_COUNT; ++i)
        {
            PixelFormat pf = static_cast<PixelFormat>(i);
            if (!accessibleOnly || isAccessible(pf))
            {
                String formatName = getFormatName(pf);
                formatNames.insert(std::make_pair(formatName.length(), formatName));
            }
        }

        // Populate the BNF expression in reverse order
        String result;
        // Note: Stupid M$ VC7.1 can't dealing operator!= with FormatNameMap::const_reverse_iterator.
        for (FormatNameMap::reverse_iterator j = formatNames.rbegin(); j != formatNames.rend(); ++j)
        {
            if (!result.empty())
                result += " | ";
            result += "'" + j->second + "'";
        }

        return result;
    }
    //-----------------------------------------------------------------------
    PixelFormat PixelUtil::getFormatForBitDepths(PixelFormat fmt, UINT16 integerBits, UINT16 floatBits)
    {
        switch (integerBits)
        {
        case 16:
            switch (fmt)
            {
            case PF_R8G8B8:
            case PF_X8R8G8B8:
                return PF_R5G6B5;

            case PF_B8G8R8:
            case PF_X8B8G8R8:
                return PF_B5G6R5;

            case PF_A8R8G8B8:
            case PF_R8G8B8A8:
            case PF_A8B8G8R8:
            case PF_B8G8R8A8:
                return PF_A4R4G4B4;

            case PF_A2R10G10B10:
            case PF_A2B10G10R10:
                return PF_A1R5G5B5;

            default:
                // use original image format
                break;
            }
            break;

        case 32:
            switch (fmt)
            {
            case PF_R5G6B5:
                return PF_X8R8G8B8;

            case PF_B5G6R5:
                return PF_X8B8G8R8;

            case PF_A4R4G4B4:
                return PF_A8R8G8B8;

            case PF_A1R5G5B5:
                return PF_A2R10G10B10;

            default:
                // use original image format
                break;
            }
            break;

        default:
            // use original image format
            break;
        }

        switch (floatBits)
        {
        case 16:
            switch (fmt)
            {
            case PF_FLOAT32_R:
                return PF_FLOAT16_R;

            case PF_FLOAT32_RGB:
                return PF_FLOAT16_RGB;

            case PF_FLOAT32_RGBA:
                return PF_FLOAT16_RGBA;

            default:
                // use original image format
                break;
            }
            break;

        case 32:
            switch (fmt)
            {
            case PF_FLOAT16_R:
                return PF_FLOAT32_R;

            case PF_FLOAT16_RGB:
                return PF_FLOAT32_RGB;

            case PF_FLOAT16_RGBA:
                return PF_FLOAT32_RGBA;

            default:
                // use original image format
                break;
            }
            break;

        default:
            // use original image format
            break;
        }

        return fmt;
    }
    //-----------------------------------------------------------------------
    /*************************************************************************
    * Pixel packing/unpacking utilities
    */
    void PixelUtil::packColour(const Color &colour, const PixelFormat pf,  void* dest)
    {
        packColour(colour.r, colour.g, colour.b, colour.a, pf, dest);
    }
    //-----------------------------------------------------------------------
    void PixelUtil::packColour(const UINT8 r, const UINT8 g, const UINT8 b, const UINT8 a, const PixelFormat pf,  void* dest)
    {
        const PixelFormatDescription &des = getDescriptionFor(pf);
        if(des.flags & PFF_NATIVEENDIAN) {
            // Shortcut for integer formats packing
            unsigned int value = ((Bitwise::fixedToFixed(r, 8, des.rbits)<<des.rshift) & des.rmask) |
                ((Bitwise::fixedToFixed(g, 8, des.gbits)<<des.gshift) & des.gmask) |
                ((Bitwise::fixedToFixed(b, 8, des.bbits)<<des.bshift) & des.bmask) |
                ((Bitwise::fixedToFixed(a, 8, des.abits)<<des.ashift) & des.amask);
            // And write to memory
            Bitwise::intWrite(dest, des.elemBytes, value);
        } else {
            // Convert to float
            packColour((float)r/255.0f,(float)g/255.0f,(float)b/255.0f,(float)a/255.0f, pf, dest);
        }
    }
    //-----------------------------------------------------------------------
    void PixelUtil::packColour(const float r, const float g, const float b, const float a, const PixelFormat pf,  void* dest)
    {
        // Catch-it-all here
        const PixelFormatDescription &des = getDescriptionFor(pf);
        if(des.flags & PFF_NATIVEENDIAN) {
            // Do the packing
            //std::cerr << dest << " " << r << " " << g <<  " " << b << " " << a << std::endl;
            const unsigned int value = ((Bitwise::floatToFixed(r, des.rbits)<<des.rshift) & des.rmask) |
                ((Bitwise::floatToFixed(g, des.gbits)<<des.gshift) & des.gmask) |
                ((Bitwise::floatToFixed(b, des.bbits)<<des.bshift) & des.bmask) |
                ((Bitwise::floatToFixed(a, des.abits)<<des.ashift) & des.amask);
            // And write to memory
            Bitwise::intWrite(dest, des.elemBytes, value);
        } else {
            switch(pf)
            {
            case PF_FLOAT32_R:
                ((float*)dest)[0] = r;
                break;
			case PF_FLOAT32_GR:
				((float*)dest)[0] = g;
				((float*)dest)[1] = r;
				break;
            case PF_FLOAT32_RGB:
                ((float*)dest)[0] = r;
                ((float*)dest)[1] = g;
                ((float*)dest)[2] = b;
                break;
            case PF_FLOAT32_RGBA:
                ((float*)dest)[0] = r;
                ((float*)dest)[1] = g;
                ((float*)dest)[2] = b;
                ((float*)dest)[3] = a;
                break;
            case PF_FLOAT16_R:
                ((UINT16*)dest)[0] = Bitwise::floatToHalf(r);
                break;
			case PF_FLOAT16_GR:
				((UINT16*)dest)[0] = Bitwise::floatToHalf(g);
				((UINT16*)dest)[1] = Bitwise::floatToHalf(r);
				break;
            case PF_FLOAT16_RGB:
                ((UINT16*)dest)[0] = Bitwise::floatToHalf(r);
                ((UINT16*)dest)[1] = Bitwise::floatToHalf(g);
                ((UINT16*)dest)[2] = Bitwise::floatToHalf(b);
                break;
            case PF_FLOAT16_RGBA:
                ((UINT16*)dest)[0] = Bitwise::floatToHalf(r);
                ((UINT16*)dest)[1] = Bitwise::floatToHalf(g);
                ((UINT16*)dest)[2] = Bitwise::floatToHalf(b);
                ((UINT16*)dest)[3] = Bitwise::floatToHalf(a);
                break;
            case PF_SHORT_RGB:
				((UINT16*)dest)[0] = (UINT16)Bitwise::floatToFixed(r, 16);
                ((UINT16*)dest)[1] = (UINT16)Bitwise::floatToFixed(g, 16);
                ((UINT16*)dest)[2] = (UINT16)Bitwise::floatToFixed(b, 16);
                break;
			case PF_SHORT_RGBA:
				((UINT16*)dest)[0] = (UINT16)Bitwise::floatToFixed(r, 16);
                ((UINT16*)dest)[1] = (UINT16)Bitwise::floatToFixed(g, 16);
                ((UINT16*)dest)[2] = (UINT16)Bitwise::floatToFixed(b, 16);
                ((UINT16*)dest)[3] = (UINT16)Bitwise::floatToFixed(a, 16);
				break;
			case PF_BYTE_LA:
				((UINT8*)dest)[0] = (UINT8)Bitwise::floatToFixed(r, 8);
                ((UINT8*)dest)[1] = (UINT8)Bitwise::floatToFixed(a, 8);
				break;
            default:
                // Not yet supported
                CM_EXCEPT(NotImplementedException,
                    "pack to "+getFormatName(pf)+" not implemented");
                break;
            }
        }
    }
    //-----------------------------------------------------------------------
    void PixelUtil::unpackColour(Color *colour, PixelFormat pf,  const void* src)
    {
        unpackColour(&colour->r, &colour->g, &colour->b, &colour->a, pf, src);
    }
    //-----------------------------------------------------------------------
    void PixelUtil::unpackColour(UINT8 *r, UINT8 *g, UINT8 *b, UINT8 *a, PixelFormat pf,  const void* src)
    {
        const PixelFormatDescription &des = getDescriptionFor(pf);
        if(des.flags & PFF_NATIVEENDIAN) {
            // Shortcut for integer formats unpacking
            const unsigned int value = Bitwise::intRead(src, des.elemBytes);
            if(des.flags & PFF_LUMINANCE)
            {
                // Luminance format -- only rbits used
                *r = *g = *b = (UINT8)Bitwise::fixedToFixed(
                    (value & des.rmask)>>des.rshift, des.rbits, 8);
            }
            else
            {
                *r = (UINT8)Bitwise::fixedToFixed((value & des.rmask)>>des.rshift, des.rbits, 8);
                *g = (UINT8)Bitwise::fixedToFixed((value & des.gmask)>>des.gshift, des.gbits, 8);
                *b = (UINT8)Bitwise::fixedToFixed((value & des.bmask)>>des.bshift, des.bbits, 8);
            }
            if(des.flags & PFF_HASALPHA)
            {
                *a = (UINT8)Bitwise::fixedToFixed((value & des.amask)>>des.ashift, des.abits, 8);
            }
            else
            {
                *a = 255; // No alpha, default a component to full
            }
        } else {
            // Do the operation with the more generic floating point
            float rr, gg, bb, aa;
            unpackColour(&rr,&gg,&bb,&aa, pf, src);
            *r = (UINT8)Bitwise::floatToFixed(rr, 8);
            *g = (UINT8)Bitwise::floatToFixed(gg, 8);
            *b = (UINT8)Bitwise::floatToFixed(bb, 8);
            *a = (UINT8)Bitwise::floatToFixed(aa, 8);
        }
    }
    //-----------------------------------------------------------------------
    void PixelUtil::unpackColour(float *r, float *g, float *b, float *a,
        PixelFormat pf,  const void* src)
    {
        const PixelFormatDescription &des = getDescriptionFor(pf);
        if(des.flags & PFF_NATIVEENDIAN) {
            // Shortcut for integer formats unpacking
            const unsigned int value = Bitwise::intRead(src, des.elemBytes);
            if(des.flags & PFF_LUMINANCE)
            {
                // Luminance format -- only rbits used
                *r = *g = *b = Bitwise::fixedToFloat(
                    (value & des.rmask)>>des.rshift, des.rbits);
            }
            else
            {
                *r = Bitwise::fixedToFloat((value & des.rmask)>>des.rshift, des.rbits);
                *g = Bitwise::fixedToFloat((value & des.gmask)>>des.gshift, des.gbits);
                *b = Bitwise::fixedToFloat((value & des.bmask)>>des.bshift, des.bbits);
            }
            if(des.flags & PFF_HASALPHA)
            {
                *a = Bitwise::fixedToFloat((value & des.amask)>>des.ashift, des.abits);
            }
            else
            {
                *a = 1.0f; // No alpha, default a component to full
            }
        } else {
            switch(pf)
            {
            case PF_FLOAT32_R:
                *r = *g = *b = ((float*)src)[0];
                *a = 1.0f;
                break;
			case PF_FLOAT32_GR:
				*g = ((float*)src)[0];
				*r = *b = ((float*)src)[1];
				*a = 1.0f;
				break;
            case PF_FLOAT32_RGB:
                *r = ((float*)src)[0];
                *g = ((float*)src)[1];
                *b = ((float*)src)[2];
                *a = 1.0f;
                break;
            case PF_FLOAT32_RGBA:
                *r = ((float*)src)[0];
                *g = ((float*)src)[1];
                *b = ((float*)src)[2];
                *a = ((float*)src)[3];
                break;
            case PF_FLOAT16_R:
                *r = *g = *b = Bitwise::halfToFloat(((UINT16*)src)[0]);
                *a = 1.0f;
                break;
			case PF_FLOAT16_GR:
				*g = Bitwise::halfToFloat(((UINT16*)src)[0]);
				*r = *b = Bitwise::halfToFloat(((UINT16*)src)[1]);
				*a = 1.0f;
				break;
            case PF_FLOAT16_RGB:
                *r = Bitwise::halfToFloat(((UINT16*)src)[0]);
                *g = Bitwise::halfToFloat(((UINT16*)src)[1]);
                *b = Bitwise::halfToFloat(((UINT16*)src)[2]);
                *a = 1.0f;
                break;
            case PF_FLOAT16_RGBA:
                *r = Bitwise::halfToFloat(((UINT16*)src)[0]);
                *g = Bitwise::halfToFloat(((UINT16*)src)[1]);
                *b = Bitwise::halfToFloat(((UINT16*)src)[2]);
                *a = Bitwise::halfToFloat(((UINT16*)src)[3]);
                break;
			case PF_SHORT_RGB:
				*r = Bitwise::fixedToFloat(((UINT16*)src)[0], 16);
                *g = Bitwise::fixedToFloat(((UINT16*)src)[1], 16);
				*b = Bitwise::fixedToFloat(((UINT16*)src)[2], 16);
				*a = 1.0f;
				break;
			case PF_SHORT_RGBA:
				*r = Bitwise::fixedToFloat(((UINT16*)src)[0], 16);
                *g = Bitwise::fixedToFloat(((UINT16*)src)[1], 16);
				*b = Bitwise::fixedToFloat(((UINT16*)src)[2], 16);
				*a = Bitwise::fixedToFloat(((UINT16*)src)[3], 16);
				break;
			case PF_BYTE_LA:
				*r = *g = *b = Bitwise::fixedToFloat(((UINT8*)src)[0], 8);
				*a = Bitwise::fixedToFloat(((UINT8*)src)[1], 8);
				break;
            default:
                // Not yet supported
                CM_EXCEPT(NotImplementedException,
                    "unpack from "+getFormatName(pf)+" not implemented");
                break;
            }
        }
    }
    //-----------------------------------------------------------------------
    /* Convert pixels from one format to another */
    void PixelUtil::bulkPixelConversion(void *srcp, PixelFormat srcFormat,
        void *destp, PixelFormat dstFormat, unsigned int count)
    {
        PixelData src(count, 1, 1, srcFormat, srcp),
				 dst(count, 1, 1, dstFormat, destp);

        bulkPixelConversion(src, dst);
    }
    //-----------------------------------------------------------------------
    void PixelUtil::bulkPixelConversion(const PixelData &src, const PixelData &dst)
    {
        assert(src.getWidth() == dst.getWidth() &&
			   src.getHeight() == dst.getHeight() &&
			   src.getDepth() == dst.getDepth());

		// Check for compressed formats, we don't support decompression, compression or recoding
		if(PixelUtil::isCompressed(src.format) || PixelUtil::isCompressed(dst.format))
		{
			if(src.format == dst.format)
			{
				memcpy(dst.data, src.data, src.getConsecutiveSize());
				return;
			}
			else
			{
				CM_EXCEPT(NotImplementedException, "This method can not be used to compress or decompress images");
			}
		}

        // The easy case
        if(src.format == dst.format) {
            // Everything consecutive?
            if(src.isConsecutive() && dst.isConsecutive())
            {
				memcpy(dst.data, src.data, src.getConsecutiveSize());
                return;
            }

            const size_t srcPixelSize = PixelUtil::getNumElemBytes(src.format);
            const size_t dstPixelSize = PixelUtil::getNumElemBytes(dst.format);
            UINT8 *srcptr = static_cast<UINT8*>(src.data)
                + (src.left + src.top * src.rowPitch + src.front * src.slicePitch) * srcPixelSize;
            UINT8 *dstptr = static_cast<UINT8*>(dst.data)
				+ (dst.left + dst.top * dst.rowPitch + dst.front * dst.slicePitch) * dstPixelSize;

            // Calculate pitches+skips in bytes
            const size_t srcRowPitchBytes = src.rowPitch*srcPixelSize;
            //const size_t srcRowSkipBytes = src.getRowSkip()*srcPixelSize;
            const size_t srcSliceSkipBytes = src.getSliceSkip()*srcPixelSize;

            const size_t dstRowPitchBytes = dst.rowPitch*dstPixelSize;
            //const size_t dstRowSkipBytes = dst.getRowSkip()*dstPixelSize;
            const size_t dstSliceSkipBytes = dst.getSliceSkip()*dstPixelSize;

            // Otherwise, copy per row
            const size_t rowSize = src.getWidth()*srcPixelSize;
            for(size_t z=src.front; z<src.back; z++)
            {
                for(size_t y=src.top; y<src.bottom; y++)
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
		// Converting to PF_X8R8G8B8 is exactly the same as converting to
		// PF_A8R8G8B8. (same with PF_X8B8G8R8 and PF_A8B8G8R8)
		if(dst.format == PF_X8R8G8B8 || dst.format == PF_X8B8G8R8)
		{
			// Do the same conversion, with PF_A8R8G8B8, which has a lot of
			// optimized conversions
			PixelData tempdst = dst;
			tempdst.format = dst.format==PF_X8R8G8B8?PF_A8R8G8B8:PF_A8B8G8R8;
			bulkPixelConversion(src, tempdst);
			return;
		}
		// Converting from PF_X8R8G8B8 is exactly the same as converting from
		// PF_A8R8G8B8, given that the destination format does not have alpha.
		if((src.format == PF_X8R8G8B8||src.format == PF_X8B8G8R8) && !hasAlpha(dst.format))
		{
			// Do the same conversion, with PF_A8R8G8B8, which has a lot of
			// optimized conversions
			PixelData tempsrc = src;
			tempsrc.format = src.format==PF_X8R8G8B8?PF_A8R8G8B8:PF_A8B8G8R8;
			bulkPixelConversion(tempsrc, dst);
			return;
		}

        const size_t srcPixelSize = PixelUtil::getNumElemBytes(src.format);
        const size_t dstPixelSize = PixelUtil::getNumElemBytes(dst.format);
        UINT8 *srcptr = static_cast<UINT8*>(src.data)
            + (src.left + src.top * src.rowPitch + src.front * src.slicePitch) * srcPixelSize;
        UINT8 *dstptr = static_cast<UINT8*>(dst.data)
            + (dst.left + dst.top * dst.rowPitch + dst.front * dst.slicePitch) * dstPixelSize;
		
		// Old way, not taking into account box dimensions
		//UINT8 *srcptr = static_cast<UINT8*>(src.data), *dstptr = static_cast<UINT8*>(dst.data);

        // Calculate pitches+skips in bytes
        const size_t srcRowSkipBytes = src.getRowSkip()*srcPixelSize;
        const size_t srcSliceSkipBytes = src.getSliceSkip()*srcPixelSize;
        const size_t dstRowSkipBytes = dst.getRowSkip()*dstPixelSize;
        const size_t dstSliceSkipBytes = dst.getSliceSkip()*dstPixelSize;

        // The brute force fallback
        float r,g,b,a;
        for(size_t z=src.front; z<src.back; z++)
        {
            for(size_t y=src.top; y<src.bottom; y++)
            {
                for(size_t x=src.left; x<src.right; x++)
                {
                    unpackColour(&r, &g, &b, &a, src.format, srcptr);
                    packColour(r, g, b, a, dst.format, dstptr);
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

	void PixelUtil::scale(const PixelData &src, const PixelData &scaled, Filter filter)
	{
		assert(PixelUtil::isAccessible(src.format));
		assert(PixelUtil::isAccessible(scaled.format));

		PixelData temp;
		switch (filter) 
		{
		default:
		case FILTER_NEAREST:
			if(src.format == scaled.format) 
			{
				// No intermediate buffer needed
				temp = scaled;
			}
			else
			{
				// Allocate temporary buffer of destination size in source format 
				temp = PixelData(scaled.getWidth(), scaled.getHeight(), scaled.getDepth(), src.format);
				temp.data = malloc(temp.getConsecutiveSize());
			}
			// super-optimized: no conversion
			switch (PixelUtil::getNumElemBytes(src.format)) 
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
				// never reached
				assert(false);
			}
			if(temp.data != scaled.data)
			{
				// Blit temp buffer
				PixelUtil::bulkPixelConversion(temp, scaled);

				free(temp.data);
			}
			break;

		case FILTER_LINEAR:
		case FILTER_BILINEAR:
			switch (src.format) 
			{
			case PF_L8: case PF_A8: case PF_BYTE_LA:
			case PF_R8G8B8: case PF_B8G8R8:
			case PF_R8G8B8A8: case PF_B8G8R8A8:
			case PF_A8B8G8R8: case PF_A8R8G8B8:
			case PF_X8B8G8R8: case PF_X8R8G8B8:
				if(src.format == scaled.format) 
				{
					// No intermediate buffer needed
					temp = scaled;
				}
				else
				{
					// Allocate temp buffer of destination size in source format 
					temp = PixelData(scaled.getWidth(), scaled.getHeight(), scaled.getDepth(), src.format);
					temp.data = malloc(temp.getConsecutiveSize());
				}
				// super-optimized: byte-oriented math, no conversion
				switch (PixelUtil::getNumElemBytes(src.format)) 
				{
				case 1: LinearResampler_Byte<1>::scale(src, temp); break;
				case 2: LinearResampler_Byte<2>::scale(src, temp); break;
				case 3: LinearResampler_Byte<3>::scale(src, temp); break;
				case 4: LinearResampler_Byte<4>::scale(src, temp); break;
				default:
					// never reached
					assert(false);
				}
				if(temp.data != scaled.data)
				{
					// Blit temp buffer
					PixelUtil::bulkPixelConversion(temp, scaled);
					free(temp.data);
				}
				break;
			case PF_FLOAT32_RGB:
			case PF_FLOAT32_RGBA:
				if (scaled.format == PF_FLOAT32_RGB || scaled.format == PF_FLOAT32_RGBA)
				{
					// float32 to float32, avoid unpack/repack overhead
					LinearResampler_Float32::scale(src, scaled);
					break;
				}
				// else, fall through
			default:
				// non-optimized: floating-point math, performs conversion but always works
				LinearResampler::scale(src, scaled);
			}
			break;
		}
	}

    Color PixelData::getColourAt(size_t x, size_t y, size_t z)
    {
        Color cv;

        unsigned char pixelSize = PixelUtil::getNumElemBytes(format);
        size_t pixelOffset = pixelSize * (z * slicePitch + y * rowPitch + x);
        PixelUtil::unpackColour(&cv, format, (unsigned char *)data + pixelOffset);

        return cv;
    }

    void PixelData::setColourAt(Color const &cv, size_t x, size_t y, size_t z)
    {
        unsigned char pixelSize = PixelUtil::getNumElemBytes(format);
        size_t pixelOffset = pixelSize * (z * slicePitch + y * rowPitch + x);
        PixelUtil::packColour(cv, format, (unsigned char *)data + pixelOffset);
    }

}

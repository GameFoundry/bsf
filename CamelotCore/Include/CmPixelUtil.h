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
#ifndef _PixelFormat_H__
#define _PixelFormat_H__

#include "CmPrerequisites.h"
#include "CmPixelData.h"

namespace CamelotFramework {
    /**
     * Some utility functions for packing and unpacking pixel data
     */
    class CM_EXPORT PixelUtil {
    public:
        /** Returns the size in bytes of an element of the given pixel format.
         @returns
               The size in bytes of an element. See Remarks.
         @remarks
               Passing PF_UNKNOWN will result in returning a size of 0 bytes.
        */
        static UINT32 getNumElemBytes( PixelFormat format );

        /** Returns the size in bits of an element of the given pixel format.
          @returns
               The size in bits of an element. See Remarks.
           @remarks
               Passing PF_UNKNOWN will result in returning a size of 0 bits.
        */
        static UINT32 getNumElemBits( PixelFormat format );

		/** Returns the size in memory of a region with the given extents and pixel
			format with consecutive memory layout.
			@param width
				The width of the area
			@param height
				The height of the area
			@param depth
				The depth of the area
			@param format
				The format of the area
		  	@returns
		  		The size in bytes
			@remarks
				In case that the format is non-compressed, this simply returns
				width*height*depth*PixelUtil::getNumElemBytes(format). In the compressed
				case, this does serious magic.
		*/
		static UINT32 getMemorySize(UINT32 width, UINT32 height, UINT32 depth, PixelFormat format);
		
        /** Returns the property flags for this pixel format
          @returns
               A bitfield combination of PFF_HASALPHA, PFF_ISCOMPRESSED,
               PFF_FLOAT, PFF_DEPTH, PFF_NATIVEENDIAN, PFF_LUMINANCE
          @remarks
               This replaces the seperate functions for formatHasAlpha, formatIsFloat, ...
        */
        static unsigned int getFlags( PixelFormat format );

        /** Shortcut method to determine if the format has an alpha component */
        static bool hasAlpha(PixelFormat format);
        /** Shortcut method to determine if the format is floating point */
        static bool isFloatingPoint(PixelFormat format);
        /** Shortcut method to determine if the format is compressed */
        static bool isCompressed(PixelFormat format);
        /** Shortcut method to determine if the format is a depth format. */
        static bool isDepth(PixelFormat format);
        /** Shortcut method to determine if the format is in native endian format. */
        static bool isNativeEndian(PixelFormat format);
		
		/** Return wether a certain image extent is valid for this image format.
			@param width
				The width of the area
			@param height
				The height of the area
			@param depth
				The depth of the area
			@param format
				The format of the area
			@remarks For non-compressed formats, this is always true. For DXT formats,
			only sizes with a width and height multiple of 4 and depth 1 are allowed.
		*/
		static bool isValidExtent(size_t width, size_t height, size_t depth, PixelFormat format);

        /** Gives the number of bits (RGBA) for a format. See remarks.          
          @remarks      For non-colour formats (dxt, depth) this returns [0,0,0,0].
        */
        static void getBitDepths(PixelFormat format, int rgba[4]);

		/** Gives the masks for the R, G, B and A component
		  @note			Only valid for native endian formats
        */
        static void getBitMasks(PixelFormat format, UINT32 rgba[4]);

		/** Gives the bit shifts for R, G, B and A component
		@note			Only valid for native endian formats
		*/
		static void getBitShifts(PixelFormat format, unsigned char rgba[4]);

        /** Gets the name of an image format
        */
        static String getFormatName(PixelFormat srcformat);

        /** Returns wether the format can be packed or unpacked with the packColour()
        and unpackColour() functions. This is generally not true for compressed and
        depth formats as they are special. It can only be true for formats with a
        fixed element size.
          @returns 
               true if yes, otherwise false
        */
        static bool isAccessible(PixelFormat srcformat);
        
        /** Returns the component type for a certain pixel format. Returns PCT_BYTE
            in case there is no clear component type like with compressed formats.
            This is one of PCT_BYTE, PCT_SHORT, PCT_FLOAT16, PCT_FLOAT32.
        */
        static PixelComponentType getComponentType(PixelFormat fmt);
        
        /** Returns the component count for a certain pixel format. Returns 3(no alpha) or 
            4 (has alpha) in case there is no clear component type like with compressed formats.
         */
        static UINT32 getComponentCount(PixelFormat fmt);

        /** Gets the format from given name.
            @param  name            The string of format name
            @param  accessibleOnly  If true, non-accessible format will treat as invalid format,
                                    otherwise, all supported format are valid.
            @param  caseSensitive   Should be set true if string match should use case sensitivity.
            @returns                The format match the format name, or PF_UNKNOWN if is invalid name.
        */
        static PixelFormat getFormatFromName(const String& name, bool accessibleOnly = false, bool caseSensitive = false);

        /** Gets the BNF expression of the pixel-formats.
            @note                   The string returned by this function is intented to use as a BNF expression
                                    to work with Compiler2Pass.
            @param  accessibleOnly  If true, only accessible pixel format will take into account, otherwise all
                                    pixel formats list in PixelFormat enumeration will being returned.
            @returns                A string contains the BNF expression.
        */
        static String getBNFExpressionOfPixelFormats(bool accessibleOnly = false);

		/** Returns the maximum number of Mipmaps that can be generated until we reach
			the mininum format possible. This does not count the base level.
			@param width
				The width of the area
			@param height
				The height of the area
			@param depth
				The depth of the area
			@param format
				The format of the area
			@remarks
				In case that the format is non-compressed, this simply returns
				how many times we can divide this texture in 2 until we reach 1x1.
				For compressed formats, constraints apply on minimum size and alignment
				so this might differ.
		*/
		static UINT32 getMaxMipmaps(UINT32 width, UINT32 height, UINT32 depth, PixelFormat format);

        /** Pack a colour value to memory
        	@param colour	The colour
        	@param pf		Pixelformat in which to write the colour
        	@param dest		Destination memory location
        */
        static void packColour(const Color &colour, const PixelFormat pf,  void* dest);
        /** Pack a colour value to memory
        	@param r,g,b,a	The four colour components, range 0x00 to 0xFF
        	@param pf		Pixelformat in which to write the colour
        	@param dest		Destination memory location
        */
        static void packColour(const UINT8 r, const UINT8 g, const UINT8 b, const UINT8 a, const PixelFormat pf,  void* dest);
         /** Pack a colour value to memory
        	@param r,g,b,a	The four colour components, range 0.0f to 1.0f
        					(an exception to this case exists for floating point pixel
        					formats, which don't clamp to 0.0f..1.0f)
        	@param pf		Pixelformat in which to write the colour
        	@param dest		Destination memory location
        */
        static void packColour(const float r, const float g, const float b, const float a, const PixelFormat pf,  void* dest);

        /** Unpack a colour value from memory
        	@param colour	The colour is returned here
        	@param pf		Pixelformat in which to read the colour
        	@param src		Source memory location
        */
        static void unpackColour(Color *colour, PixelFormat pf,  const void* src);
        /** Unpack a colour value from memory
        	@param r,g,b,a	The colour is returned here (as byte)
        	@param pf		Pixelformat in which to read the colour
        	@param src		Source memory location
        	@remarks 	This function returns the colour components in 8 bit precision,
        		this will lose precision when coming from PF_A2R10G10B10 or floating
        		point formats.  
        */
        static void unpackColour(UINT8 *r, UINT8 *g, UINT8 *b, UINT8 *a, PixelFormat pf,  const void* src);
        /** Unpack a colour value from memory
        	@param r,g,b,a	The colour is returned here (as float)
        	@param pf		Pixelformat in which to read the colour
        	@param src		Source memory location
        */
        static void unpackColour(float *r, float *g, float *b, float *a, PixelFormat pf,  const void* src); 
        
      	/** Convert pixels from one format to another. No dithering or filtering is being done. Converting
          	from RGB to luminance takes the R channel. 
		 	@param	src			PixelBox containing the source pixels, pitches and format
		 	@param	dst			PixelBox containing the destination pixels, pitches and format
		 	@remarks The source and destination boxes must have the same
         	dimensions. In case the source and destination format match, a plain copy is done.
        */
        static void bulkPixelConversion(const PixelData &src, const PixelData &dst);

		enum Filter
		{
			FILTER_NEAREST,
			FILTER_LINEAR,
			FILTER_BILINEAR,
			FILTER_BOX,
			FILTER_TRIANGLE,
			FILTER_BICUBIC
		};

		/** Scale a 1D, 2D or 3D image volume. 
			@param 	src			PixelBox containing the source pointer, dimensions and format
			@param 	dst			PixelBox containing the destination pointer, dimensions and format
			@param 	filter		Which filter to use
			@remarks 	This function can do pixel format conversion in the process.
			@note	dst and src can point to the same PixelBox object without any problem
		*/
		static void scale(const PixelData &src, const PixelData &dst, Filter filter = FILTER_BILINEAR);

		/** Does gamma adjustment.
            @note
                Basic algo taken from Titan Engine, copyright (c) 2000 Ignacio 
                Castano Iguado
        */
        static void applyGamma(UINT8 *buffer, float gamma, size_t size, UINT8 bpp);
    };
	/** @} */
	/** @} */

}

#endif

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
#ifndef __HardwarePixelBuffer__
#define __HardwarePixelBuffer__

// Precompiler options
#include "CmPrerequisites.h"
#include "CmHardwareBuffer.h"
#include "CmPixelUtil.h"

namespace CamelotEngine {

	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup RenderSystem
	*  @{
	*/
	/** Specialisation of HardwareBuffer for a pixel buffer. The
    	HardwarePixelbuffer abstracts an 1D, 2D or 3D quantity of pixels
    	stored by the rendering API. The buffer can be located on the card
    	or in main memory depending on its usage. One mipmap level of a
    	texture is an example of a HardwarePixelBuffer.
    */
    class CM_EXPORT HardwarePixelBuffer : public HardwareBuffer
    {
    protected: 
        // Extents
        UINT32 mWidth, mHeight, mDepth;
        // Pitches (offsets between rows and slices)
        UINT32 mRowPitch, mSlicePitch;
        // Internal format
        PixelFormat mFormat;
        // Currently locked region (local coords)
        PixelData mCurrentLock;
		// The current locked box of this surface (entire surface coords)
		Box mLockedBox;
        
        /// Internal implementation of lock(), must be overridden in subclasses
        virtual PixelData lockImpl(const Box lockBox,  LockOptions options) = 0;

        /// Internal implementation of lock(), do not OVERRIDE or CALL this
        /// for HardwarePixelBuffer implementations, but override the previous method
        virtual void* lockImpl(UINT32 offset, UINT32 length, LockOptions options);

		friend class RenderTexture;
    public:
        /// Should be called by HardwareBufferManager
        HardwarePixelBuffer(UINT32 mWidth, UINT32 mHeight, UINT32 mDepth,
                PixelFormat mFormat,
                HardwareBuffer::Usage usage, bool useSystemMemory);
        ~HardwarePixelBuffer();

        /** make every lock method from HardwareBuffer available.
        See http://www.research.att.com/~bs/bs_faq2.html#overloadderived
        */
        using HardwareBuffer::lock;	

        /** Lock the buffer for (potentially) reading / writing.
		    @param lockBox Region of the buffer to lock
		    @param options Locking options
		    @returns PixelBox containing the locked region, the pitches and
		    	the pixel format
		*/
		virtual const PixelData& lock(const Box& lockBox, LockOptions options);
		/// @copydoc HardwareBuffer::lock
        virtual void* lock(UINT32 offset, UINT32 length, LockOptions options);

		/** Get the current locked region. This is the same value as returned
		    by lock(const Image::Box, LockOptions)
		    @returns PixelBox containing the locked region
		*/        
        const PixelData& getCurrentLock();
		
		/// @copydoc HardwareBuffer::readData
		virtual void readData(UINT32 offset, UINT32 length, void* pDest);
		/// @copydoc HardwareBuffer::writeData
		virtual void writeData(UINT32 offset, UINT32 length, const void* pSource,
				bool discardWholeBuffer = false);
        
        /** Copies a box from another PixelBuffer to a region of the 
        	this PixelBuffer. 
			@param dst		Source pixel buffer
        	@param srcBox	Image::Box describing the source region in src
        	@param dstBox	Image::Box describing the destination region in this buffer
			@remarks The source and destination regions dimensions don't have to match, in which
		   	case scaling is done. This scaling is generally done using a bilinear filter in hardware,
            but it is faster to pass the source image in the right dimensions.
			@note Only call this function when both  buffers are unlocked. 
         */        
        virtual void blit(const HardwarePixelBufferPtr &src, const Box &srcBox, const Box &dstBox);

		/** Convenience function that blits the entire source pixel buffer to this buffer. 
			If source and destination dimensions don't match, scaling is done.
			@param src		PixelBox containing the source pixels and format in memory
			@note Only call this function when the buffer is unlocked. 
		*/
		void blit(const HardwarePixelBufferPtr &src); 
		
		/** Copies a region from normal memory to a region of this pixelbuffer. The source
			image can be in any pixel format supported by OGRE, and in any size. 
		   	@param src		PixelBox containing the source pixels and format in memory
		   	@param dstBox	Image::Box describing the destination region in this buffer
            @remarks The source and destination regions dimensions don't have to match, in which
            case scaling is done. This scaling is generally done using a bilinear filter in hardware,
            but it is faster to pass the source image in the right dimensions.
			@note Only call this function when the buffer is unlocked. 
		*/
		virtual void blitFromMemory(const PixelData &src, const Box &dstBox) = 0;
		
		/** Convenience function that blits a pixelbox from memory to the entire 
			buffer. The source image is scaled as needed.
			@param src		PixelBox containing the source pixels and format in memory
			@note Only call this function when the buffer is unlocked. 
		*/
		void blitFromMemory(const PixelData &src)
		{
			blitFromMemory(src, Box(0,0,0,mWidth,mHeight,mDepth));
		}
		
		/** Copies a region of this pixelbuffer to normal memory.
		   	@param srcBox	Image::Box describing the source region of this buffer
		   	@param dst		PixelBox describing the destination pixels and format in memory
		   	@remarks The source and destination regions don't have to match, in which
		   	case scaling is done.
			@note Only call this function when the buffer is unlocked. 
		 */
		virtual void blitToMemory(const Box &srcBox, const PixelData &dst) = 0;

		/** Convience function that blits this entire buffer to a pixelbox.
			The image is scaled as needed.
			@param src		PixelBox containing the source pixels and format in memory
			@note Only call this function when the buffer is unlocked. 
		*/
		void blitToMemory(const PixelData &dst)
		{
			blitToMemory(Box(0,0,0,mWidth,mHeight,mDepth), dst);
		}
        
        /// Gets the width of this buffer
        UINT32 getWidth() const { return mWidth; }
        /// Gets the height of this buffer
        UINT32 getHeight() const { return mHeight; }
        /// Gets the depth of this buffer
        UINT32 getDepth() const { return mDepth; }
        /// Gets the native pixel format of this buffer
        PixelFormat getFormat() const { return mFormat; }
    };

	/** @} */
}
#endif


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
#ifndef __D3D9PIXELBUFFER_H__
#define __D3D9PIXELBUFFER_H__

#include "CmD3D9Prerequisites.h"
#include "CmHardwarePixelBuffer.h"

namespace CamelotEngine {

	class D3D9Texture;
	class D3D9RenderTexture;

	class CM_D3D9_EXPORT D3D9HardwarePixelBuffer : public HardwarePixelBuffer
	{
	public:
		D3D9HardwarePixelBuffer(HardwareBuffer::Usage usage, 
			D3D9Texture* ownerTexture);
		~D3D9HardwarePixelBuffer();

		/// Call this to associate a D3D surface or volume with this pixel buffer
		void bind(IDirect3DDevice9 *dev, IDirect3DSurface9 *mSurface, IDirect3DSurface9* fsaaSurface,
			bool writeGamma, UINT32 fsaa, const String& srcName, IDirect3DBaseTexture9 *mipTex);
		void bind(IDirect3DDevice9 *dev, IDirect3DVolume9 *mVolume, IDirect3DBaseTexture9 *mipTex);

		/// @copydoc HardwarePixelBuffer::blit
		void blit(const HardwarePixelBufferPtr &src, const Box &srcBox, const Box &dstBox);

		/// @copydoc HardwarePixelBuffer::blitFromMemory
		void blitFromMemory(const PixelData &src, const Box &dstBox);

		/// @copydoc HardwarePixelBuffer::blitToMemory
		void blitToMemory(const Box &srcBox, const PixelData &dst);

		/// Internal function to update mipmaps on update of level 0
		void _genMipmaps(IDirect3DBaseTexture9* mipTex);

		/// Function to set mipmap generation
		void _setMipmapping(bool doMipmapGen, bool HWMipmaps);

		/// Accessor for surface
		IDirect3DSurface9 *getSurface(IDirect3DDevice9* d3d9Device);

		/// Accessor for AA surface
		IDirect3DSurface9 *getFSAASurface(IDirect3DDevice9* d3d9Device);

		/// Release surfaces held by this pixel buffer.
		void releaseSurfaces(IDirect3DDevice9* d3d9Device);

		/// Destroy resources associated with the given device.
		void destroyBufferResources(IDirect3DDevice9* d3d9Device);

		// Called when device state is changing. Access to any device should be locked.
		// Relevant for multi thread application.
		static void lockDeviceAccess();

		// Called when device state change completed. Access to any device is allowed.
		// Relevant for multi thread application.
		static void unlockDeviceAccess();
	protected:		
		struct BufferResources
		{			
			/// Surface abstracted by this buffer
			IDirect3DSurface9* surface;
			/// AA Surface abstracted by this buffer
			IDirect3DSurface9* fSAASurface;
			/// Volume abstracted by this buffer
			IDirect3DVolume9* volume;
			/// Temporary surface in main memory if direct locking of mSurface is not possible
			IDirect3DSurface9* tempSurface;
			/// Temporary volume in main memory if direct locking of mVolume is not possible
			IDirect3DVolume9* tempVolume;
			/// Mip map texture.
			IDirect3DBaseTexture9 *mipTex;			
		};

		typedef map<IDirect3DDevice9*, BufferResources*>::type	DeviceToBufferResourcesMap;
		typedef DeviceToBufferResourcesMap::iterator			DeviceToBufferResourcesIterator;

		/// Map between device to buffer resources.
		DeviceToBufferResourcesMap	mMapDeviceToBufferResources;
				
		/// Mipmapping
		bool mDoMipmapGen;
		bool mHWMipmaps;

		// The owner texture if exists.
		D3D9Texture* mOwnerTexture;
		
		// The current lock flags of this surface.
		DWORD mLockFlags;

		// Device access mutex.
		CM_STATIC_MUTEX(msDeviceAccessMutex)		
	protected:
		/// Lock a box
		PixelData lockImpl(const Box lockBox,  LockOptions options);
		PixelData lockBuffer(BufferResources* bufferResources, const Box &lockBox, DWORD flags);

		/// Unlock a box
		void unlockImpl(void);
		void unlockBuffer(BufferResources* bufferResources);

		BufferResources* getBufferResources(IDirect3DDevice9* d3d9Device);
		BufferResources* createBufferResources();

		void blit(IDirect3DDevice9* d3d9Device, const HardwarePixelBufferPtr &src,
				const Box &srcBox, const Box &dstBox, 
				BufferResources* srcBufferResources, 
				BufferResources* dstBufferResources);
		void blitFromMemory(const PixelData &src, const Box &dstBox, BufferResources* dstBufferResources);

		void blitToMemory(const Box &srcBox, const PixelData &dst, BufferResources* srcBufferResources, IDirect3DDevice9* d3d9Device);
	};
};
#endif

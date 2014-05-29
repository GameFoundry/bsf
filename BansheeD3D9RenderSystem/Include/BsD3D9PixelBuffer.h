#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsPixelBuffer.h"

namespace BansheeEngine 
{
	class BS_D3D9_EXPORT D3D9PixelBuffer : public PixelBuffer
	{
	protected:
		struct BufferResources
		{
			/// Surface abstracted by this buffer
			IDirect3DSurface9* surface;
			/// Volume abstracted by this buffer
			IDirect3DVolume9* volume;
			/// Temporary surface in main memory if direct locking of mSurface is not possible
			IDirect3DSurface9* tempSurface;
			/// Temporary volume in main memory if direct locking of mVolume is not possible
			IDirect3DVolume9* tempVolume;
			/// Mip map texture.
			IDirect3DBaseTexture9 *mipTex;
		};

	public:
		D3D9PixelBuffer(GpuBufferUsage usage, D3D9Texture* ownerTexture);
		~D3D9PixelBuffer();

		/// Call this to associate a D3D surface or volume with this pixel buffer
		void bind(IDirect3DDevice9* dev, IDirect3DSurface9* mSurface,
			bool writeGamma, UINT32 multisampleCount, const String& srcName, IDirect3DBaseTexture9* mipTex);
		void bind(IDirect3DDevice9* dev, IDirect3DVolume9* mVolume, IDirect3DBaseTexture9* mipTex);
		/// Internal function to update mipmaps on update of level 0
		void genMipmaps(IDirect3DBaseTexture9* mipTex);

		/// Function to set mipmap generation
		void setMipmapping(bool doMipmapGen, bool HWMipmaps);

		/// Accessor for surface
		IDirect3DSurface9 *getSurface(IDirect3DDevice9* d3d9Device);

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
		/// Lock a box
		PixelData lockImpl(PixelVolume lockBox, GpuLockOptions options);
		PixelData lockBuffer(BufferResources* bufferResources, const PixelVolume& lockBox, DWORD flags);

		/// Unlock a box
		void unlockImpl();
		void unlockBuffer(BufferResources* bufferResources);

		BufferResources* getBufferResources(IDirect3DDevice9* d3d9Device);
		BufferResources* createBufferResources();

		void blit(IDirect3DDevice9* d3d9Device, const PixelBufferPtr& src, const PixelVolume& srcBox, const PixelVolume& dstBox, 
				BufferResources* srcBufferResources, BufferResources* dstBufferResources);

		void blitFromMemory(const PixelData& src, const PixelVolume& dstBox, BufferResources* dstBufferResources);
		void blitToMemory(const PixelVolume& srcBox, const PixelData& dst, BufferResources* srcBufferResources, IDirect3DDevice9* d3d9Device);

	protected:
		/// Map between device to buffer resources.
		Map<IDirect3DDevice9*, BufferResources*> mMapDeviceToBufferResources;

		/// Mipmapping
		bool mDoMipmapGen;
		bool mHWMipmaps;

		// The owner texture if exists.
		D3D9Texture* mOwnerTexture;

		// The current lock flags of this surface.
		DWORD mLockFlags;
	};
};
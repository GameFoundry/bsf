#pragma once

#include "CmD3D9Prerequisites.h"
#include "CmTexture.h"
#include "CmRenderTexture.h"
#include "CmException.h"
#include "CmD3D9PixelBuffer.h"
#include "CmD3D9Resource.h"

namespace BansheeEngine 
{
	class CM_D3D9_EXPORT D3D9Texture : public Texture, public D3D9Resource
	{
	protected:
		struct TextureResources
		{
			/// 1D/2D normal texture pointer
			IDirect3DTexture9* pNormTex;
			/// cubic texture pointer
			IDirect3DCubeTexture9* pCubeTex;
			/// Volume texture
			IDirect3DVolumeTexture9* pVolumeTex;
			/// actual texture pointer
			IDirect3DBaseTexture9* pBaseTex;
			/// Optional FSAA surface
			IDirect3DSurface9* pFSAASurface;
			/// Optional depth stencil surface
			IDirect3DSurface9* pDepthStencilSurface;
		};

	public:
		~D3D9Texture();

		/**
		 * @copydoc Texture::isBindableAsShaderResource
		 */
		bool isBindableAsShaderResource() const { return mIsBindableAsShaderResource; }

		/// retrieves a pointer to the actual texture
		IDirect3DBaseTexture9 *getTexture_internal();		
		/// retrieves a pointer to the normal 1D/2D texture
		IDirect3DTexture9 *getNormTexture_internal();
		/// retrieves a pointer to the cube texture
		IDirect3DCubeTexture9 *getCubeTexture_internal();

		/** Indicates whether the hardware gamma is actually enabled and supported. 
		@remarks
			Because hardware gamma might not actually be supported, we need to 
			ignore it sometimes. Because D3D doesn't encode sRGB in the format but
			as a sampler state, and we don't want to change the original requested
			hardware gamma flag (e.g. serialisation) we need another indicator.
		*/
		bool isHardwareGammaReadToBeUsed() const { return mHwGamma && mHwGammaReadSupported; }
					
		/// Will this texture need to be in the default pool?
		bool useDefaultPool();

		/** Return hardware pixel buffer for a surface. This buffer can then
			be used to copy data from and to a particular level of the texture.
			@param face 	Face number, in case of a cubemap texture. Must be 0
							for other types of textures.
                            For cubemaps, this is one of 
                            +X (0), -X (1), +Y (2), -Y (3), +Z (4), -Z (5)
			@param mipmap	Mipmap level. This goes from 0 for the first, largest
							mipmap level to getNumMipmaps()-1 for the smallest.
			@returns	A shared pointer to a hardware pixel buffer
			@remarks	The buffer is invalidated when the resource is unloaded or destroyed.
						Do not use it after the lifetime of the containing texture.
		*/
		PixelBufferPtr getBuffer(UINT32 face, UINT32 mipmap);

		/**
		 * @copydoc	D3D9Resource::notifyOnDeviceCreate
		 */
		virtual void notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device);

		/**
		 * @copydoc	D3D9Resource::notifyOnDeviceDestroy
		 */
		virtual void notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device);

		/**
		 * @copydoc	D3D9Resource::notifyOnDeviceLost
		 */
		virtual void notifyOnDeviceLost(IDirect3DDevice9* d3d9Device);

		/**
		 * @copydoc	D3D9Resource::notifyOnDeviceReset
		 */
		virtual void notifyOnDeviceReset(IDirect3DDevice9* d3d9Device);

	protected:	
		friend class D3D9TextureManager;
		friend class D3D9PixelBuffer;

		D3D9Texture();

		/**
		 * @copydoc Texture::initialize_internal()
		 */
		void initialize_internal();	
		
		/**
		 * @copydoc Texture::destroy_internal()
		 */
		void destroy_internal();

		/**
		 * @copydoc Texture::lock
		 */
		PixelData lockImpl(GpuLockOptions options, UINT32 mipLevel = 0, UINT32 face = 0);

		/**
		 * @copydoc Texture::unlock
		 */
		void unlockImpl();

		/**
		 * @copydoc Texture::copy
		 */
		void copyImpl(TexturePtr& target);

		/**
		 * @copydoc Texture::readData
		 */
		void readData(PixelData& dest, UINT32 mipLevel = 0, UINT32 face = 0);

		/**
		 * @copydoc Texture::writeData
		 */
		void writeData(const PixelData& src, UINT32 mipLevel = 0, UINT32 face = 0, bool discardWholeBuffer = false);

		/// internal method, create a blank normal 1D/2D texture		
		void createNormTex(IDirect3DDevice9* d3d9Device);
		/// internal method, create a blank cube texture		
		void createCubeTex(IDirect3DDevice9* d3d9Device);
		/// internal method, create a blank cube texture		
		void createVolumeTex(IDirect3DDevice9* d3d9Device);

		/// internal method, return a D3D pixel format for texture creation
		D3DFORMAT chooseD3DFormat(IDirect3DDevice9* d3d9Device);

		/// @copydoc Resource::calculateSize
		UINT32 calculateSize() const;
		/// Creates this texture resources on the specified device.
		void createInternalResources(IDirect3DDevice9* d3d9Device);
		/// internal method, set Texture class final texture protected attributes
		void setFinalAttributes(IDirect3DDevice9* d3d9Device, TextureResources* textureResources, 
			UINT32 width, UINT32 height, UINT32 depth, PixelFormat format);
		/// internal method, return the best by hardware supported filter method
		D3DTEXTUREFILTERTYPE getBestFilterMethod(IDirect3DDevice9* d3d9Device);
		/// internal method, return true if the device/texture combination can use dynamic textures
		bool canUseDynamicTextures(IDirect3DDevice9* d3d9Device, DWORD srcUsage, D3DRESOURCETYPE srcType, D3DFORMAT srcFormat);
		/// internal method, return true if the device/texture combination can auto gen. mip maps
		bool canAutoGenMipmaps(IDirect3DDevice9* d3d9Device, DWORD srcUsage, D3DRESOURCETYPE srcType, D3DFORMAT srcFormat);
		/// internal method, return true if the device/texture combination can use hardware gamma
		bool canUseHardwareGammaCorrection(IDirect3DDevice9* d3d9Device, DWORD srcUsage, D3DRESOURCETYPE srcType, D3DFORMAT srcFormat, bool forwriting);

		/// internal method, create D3D9HardwarePixelBuffers for every face and
		/// mipmap level. This method must be called after the D3D texture object was created
		void createSurfaceList(IDirect3DDevice9* d3d9Device, TextureResources* textureResources);
	 
		/// gets the texture resources attached to the given device.
		TextureResources* getTextureResources(IDirect3DDevice9* d3d9Device);

		/// allocates new texture resources structure attached to the given device.
		TextureResources* allocateTextureResources(IDirect3DDevice9* d3d9Device);

		/// frees the given texture resources.
		void freeTextureResources(IDirect3DDevice9* d3d9Device, TextureResources* textureResources);

		void determinePool();

	protected:
		/// Map between device to texture resources.
		Map<IDirect3DDevice9*, TextureResources*> mMapDeviceToTextureResources;

		/// Vector of pointers to subsurfaces
		Vector<PixelBufferPtr>	mSurfaceList;
		/// The memory pool being used
		D3DPOOL	mD3DPool;
		// Dynamic textures?
		bool mDynamicTextures;
		bool mIsBindableAsShaderResource;

		PixelBufferPtr mLockedBuffer;

		/// Is hardware gamma supported (read)?
		bool mHwGammaReadSupported;
		/// Is hardware gamma supported (write)?
		bool mHwGammaWriteSupported;
		D3DMULTISAMPLE_TYPE mFSAAType;
		DWORD mFSAAQuality;
    };
}
#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsTexture.h"
#include "BsRenderTexture.h"
#include "BsException.h"
#include "BsD3D9PixelBuffer.h"
#include "BsD3D9Resource.h"

namespace BansheeEngine 
{
	/**
	 * @brief	DirectX 9 implementation of a texture.
	 */
	class BS_D3D9_EXPORT D3D9Texture : public Texture, public D3D9Resource
	{
	protected:
		/**
		 * @brief	Container for internal resources.
		 */
		struct TextureResources
		{
			IDirect3DTexture9* pNormTex;
			IDirect3DCubeTexture9* pCubeTex;
			IDirect3DVolumeTexture9* pVolumeTex;
			IDirect3DBaseTexture9* pBaseTex;
			IDirect3DSurface9* pMultisampleSurface;
			IDirect3DSurface9* pDepthStencilSurface;
		};

	public:
		~D3D9Texture();

		/**
		 * @copydoc Texture::isBindableAsShaderResource
		 */
		bool isBindableAsShaderResource() const { return mIsBindableAsShaderResource; }

		/**
		 * @brief	Returns internal DirectX 9 texture object.
		 */
		IDirect3DBaseTexture9* getTexture_internal();	

		/**
		 * @brief	Returns internal DirectX 9 texture object pointing to a 1D or 2D texture.
		 */
		IDirect3DTexture9* getNormTexture_internal();

		/**
		 * @brief	Returns internal DirectX 9 texture object pointing to a cube texture.
		 */
		IDirect3DCubeTexture9* getCubeTexture_internal();

		/**
		 * @brief	Indicates whether the hardware gamma is enabled and supported.
		 */
		bool isHardwareGammaReadToBeUsed() const { return mHwGamma && mHwGammaReadSupported; }
					
		/**
		 * @brief	Returns a hardware pixel buffer for a certain face and level of the texture.
		 * 
		 * @param	face	Index of the texture face, if texture has more than one. Array index for
		 *					texture arrays and a cube face for cube textures.
		 * @param	mipmap	Index of the mip level. 0 being the largest mip level.
		 *
		 * @note	Cube face indices: +X (0), -X (1), +Y (2), -Y (3), +Z (4), -Z (5)
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
		void copyImpl(UINT32 srcFace, UINT32 srcMipLevel, UINT32 destFace, UINT32 destMipLevel, TexturePtr& target);

		/**
		 * @copydoc Texture::readData
		 */
		void readData(PixelData& dest, UINT32 mipLevel = 0, UINT32 face = 0);

		/**
		 * @copydoc Texture::writeData
		 */
		void writeData(const PixelData& src, UINT32 mipLevel = 0, UINT32 face = 0, bool discardWholeBuffer = false);

		/**
		 * @brief	Returns true if the texture should be allocated in the default pool.
		 */
		bool useDefaultPool();

		/**
		 * @brief	Creates a DirectX object for blank normal 1D/2D texture.
		 */
		void createNormTex(IDirect3DDevice9* d3d9Device);

		/**
		 * @brief	Creates a DirectX object for blank cube texture.
		 */
		void createCubeTex(IDirect3DDevice9* d3d9Device);

		/**
		 * @brief	Creates a DirectX object for blank volume (3D) texture.
		 */	
		void createVolumeTex(IDirect3DDevice9* d3d9Device);

		/**
		 * @brief	Selects a DirectX 9 pixel format depending on requested format
		 *			and device capabilities.
		 */
		D3DFORMAT chooseD3DFormat(IDirect3DDevice9* d3d9Device);

		/**
		 * @copydoc	Texture::calculateSize
		 */
		UINT32 calculateSize() const;

		/**
		 * @brief	Creates internal resources for the texture for the specified device.
		 */
		void createInternalResources(IDirect3DDevice9* d3d9Device);

		/**
		 * @brief	Updates texture attributes with final attributes provided by the API after
		 *			the texture has been created.
		 */
		void setFinalAttributes(IDirect3DDevice9* d3d9Device, TextureResources* textureResources, 
			UINT32 width, UINT32 height, UINT32 depth, PixelFormat format);

		/**
		 * @brief	Returns best texture filtering method.
		 */
		D3DTEXTUREFILTERTYPE getBestFilterMethod(IDirect3DDevice9* d3d9Device);

		/**
		 * @brief	Returns if the specified combination of texture type, format and usage supports dynamic texture usage.
		 */
		bool canUseDynamicTextures(IDirect3DDevice9* d3d9Device, DWORD srcUsage, D3DRESOURCETYPE srcType, D3DFORMAT srcFormat);

		/**
		 * @brief	Returns if the specified combination of texture type, format and usage supports automatic mipmap generation.
		 */
		bool canAutoGenMipmaps(IDirect3DDevice9* d3d9Device, DWORD srcUsage, D3DRESOURCETYPE srcType, D3DFORMAT srcFormat);

		/**
		 * @brief	Returns if the specified combination of texture type, format and usage supports hardware gamma.
		 */
		bool canUseHardwareGammaCorrection(IDirect3DDevice9* d3d9Device, DWORD srcUsage, D3DRESOURCETYPE srcType, D3DFORMAT srcFormat, bool forwriting);

		/**
		 * @brief	Creates a list of pixel buffers for all surfaces in the texture. This must be called after the texture
		 *			has been created.
		 */
		void createSurfaceList(IDirect3DDevice9* d3d9Device, TextureResources* textureResources);
	 
		/**
		 * @brief	Retrieves texture resources for the device, or null if they don't exist.
		 */
		TextureResources* getTextureResources(IDirect3DDevice9* d3d9Device);

		/**
		 * @brief	Allocates a new set of texture resources for the device.
		 */
		TextureResources* allocateTextureResources(IDirect3DDevice9* d3d9Device);

		/**
		 * @brief	Frees all the given texture resources.
		 */
		void freeTextureResources(IDirect3DDevice9* d3d9Device, TextureResources* textureResources);

		/**
		 * @brief	Determines the pool in which to create the texture in.
		 */
		void determinePool();

	protected:
		Map<IDirect3DDevice9*, TextureResources*> mMapDeviceToTextureResources;

		Vector<PixelBufferPtr> mSurfaceList;

		D3DPOOL	mD3DPool;
		bool mDynamicTextures;
		bool mIsBindableAsShaderResource;

		PixelBufferPtr mLockedBuffer;

		bool mHwGammaReadSupported;
		bool mHwGammaWriteSupported;
		D3DMULTISAMPLE_TYPE mMultisampleType;
		DWORD mMultisampleQuality;
    };
}
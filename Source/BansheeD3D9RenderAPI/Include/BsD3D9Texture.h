//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsTexture.h"
#include "BsD3D9PixelBuffer.h"
#include "BsD3D9Resource.h"

namespace BansheeEngine 
{
	/** @addtogroup D3D9
	 *  @{
	 */

	/**	DirectX 9 implementation of a texture. */
	class BS_D3D9_EXPORT D3D9TextureCore : public TextureCore, public D3D9Resource
	{
	protected:
		/**	Container for internal resources. */
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
		~D3D9TextureCore();

		/** @copydoc TextureCore::isBindableAsShaderResource */
		bool isBindableAsShaderResource() const override { return mIsBindableAsShaderResource; }

		/**	Returns internal DirectX 9 texture object. */
		IDirect3DBaseTexture9* getTexture_internal();	

		/**	Returns internal DirectX 9 texture object pointing to a 1D or 2D texture. */
		IDirect3DTexture9* getNormTexture_internal();

		/**	Returns internal DirectX 9 texture object pointing to a cube texture. */
		IDirect3DCubeTexture9* getCubeTexture_internal();

		/**	Indicates whether the hardware gamma is enabled and supported. */
		bool isHardwareGammaReadToBeUsed() const { return mProperties.isHardwareGammaEnabled() && mHwGammaReadSupported; }
					
		/**
		 * Returns a hardware pixel buffer for a certain face and level of the texture.
		 * 
		 * @param[in]	face	Index of the texture face, if texture has more than one. Array index for texture arrays and
		 *						a cube face for cube textures.
		 * @param[in]	mipmap	Index of the mip level. 0 being the largest mip level.
		 *
		 * @note	Cube face indices: +X (0), -X (1), +Y (2), -Y (3), +Z (4), -Z (5)
		 */
		PixelBufferPtr getBuffer(UINT32 face, UINT32 mipmap);

		/** @copydoc D3D9Resource::notifyOnDeviceCreate */
		virtual void notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device) override;

		/** @copydoc D3D9Resource::notifyOnDeviceDestroy */
		virtual void notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device) override;

		/** @copydoc D3D9Resource::notifyOnDeviceLost */
		virtual void notifyOnDeviceLost(IDirect3DDevice9* d3d9Device) override;

		/** @copydoc D3D9Resource::notifyOnDeviceReset */
		virtual void notifyOnDeviceReset(IDirect3DDevice9* d3d9Device) override;

	protected:	
		friend class D3D9TextureCoreManager;
		friend class D3D9PixelBuffer;

		D3D9TextureCore(TextureType textureType, UINT32 width, UINT32 height, UINT32 depth, UINT32 numMipmaps,
			PixelFormat format, int usage, bool hwGamma, UINT32 multisampleCount, const PixelDataPtr& initialData);

		/** @copydoc TextureCore::initialize */
		void initialize() override;
		
		/** @copydoc TextureCore::lock */
		PixelData lockImpl(GpuLockOptions options, UINT32 mipLevel = 0, UINT32 face = 0) override;

		/** @copydoc TextureCore::unlock */
		void unlockImpl() override;

		/** @copydoc TextureCore::copyImpl */
		void copyImpl(UINT32 srcFace, UINT32 srcMipLevel, UINT32 destFace, UINT32 destMipLevel, const SPtr<TextureCore>& target) override;

		/** @copydoc TextureCore::readData */
		void readData(PixelData& dest, UINT32 mipLevel = 0, UINT32 face = 0) override;

		/** @copydoc TextureCore::writeData */
		void writeData(const PixelData& src, UINT32 mipLevel = 0, UINT32 face = 0, bool discardWholeBuffer = false) override;

		/**	Returns true if the texture should be allocated in the default pool. */
		bool useDefaultPool();

		/**	Creates a DirectX object for blank normal 1D/2D texture. */
		void createNormTex(IDirect3DDevice9* d3d9Device);

		/**	Creates a DirectX object for blank cube texture. */
		void createCubeTex(IDirect3DDevice9* d3d9Device);

		/**	Creates a DirectX object for blank volume (3D) texture. */	
		void createVolumeTex(IDirect3DDevice9* d3d9Device);

		/**	Selects a DirectX 9 pixel format depending on requested format and device capabilities. */
		D3DFORMAT chooseD3DFormat(IDirect3DDevice9* d3d9Device);

		/** @copydoc Texture::calculateSize */
		UINT32 calculateSize() const;

		/**	Creates internal resources for the texture for the specified device. */
		void createInternalResources(IDirect3DDevice9* d3d9Device);

		/** Updates texture attributes with final attributes provided by the API after the texture has been created. */
		void setFinalAttributes(IDirect3DDevice9* d3d9Device, TextureResources* textureResources, 
			UINT32 width, UINT32 height, UINT32 depth, PixelFormat format);

		/**	Returns best texture filtering method. */
		D3DTEXTUREFILTERTYPE getBestFilterMethod(IDirect3DDevice9* d3d9Device);

		/**	Returns if the specified combination of texture type, format and usage supports dynamic texture usage. */
		bool canUseDynamicTextures(IDirect3DDevice9* d3d9Device, DWORD srcUsage, D3DRESOURCETYPE srcType, D3DFORMAT srcFormat);

		/**	Returns if the specified combination of texture type, format and usage supports automatic mipmap generation. */
		bool canAutoGenMipmaps(IDirect3DDevice9* d3d9Device, DWORD srcUsage, D3DRESOURCETYPE srcType, D3DFORMAT srcFormat);

		/**	Returns if the specified combination of texture type, format and usage supports hardware gamma. */
		bool canUseHardwareGammaCorrection(IDirect3DDevice9* d3d9Device, DWORD srcUsage, D3DRESOURCETYPE srcType, D3DFORMAT srcFormat, bool forwriting);

		/**
		 * Creates a list of pixel buffers for all surfaces in the texture. This must be called after the texture has been
		 * created.
		 */
		void createSurfaceList(IDirect3DDevice9* d3d9Device, TextureResources* textureResources);
	 
		/**	Retrieves texture resources for the device, or null if they don't exist. */
		TextureResources* getTextureResources(IDirect3DDevice9* d3d9Device);

		/**	Allocates a new set of texture resources for the device. */
		TextureResources* allocateTextureResources(IDirect3DDevice9* d3d9Device);

		/**	Frees all the given texture resources. */
		void freeTextureResources(IDirect3DDevice9* d3d9Device, TextureResources* textureResources);

		/**	Determines the pool in which to create the texture in. */
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

	/** @} */
}
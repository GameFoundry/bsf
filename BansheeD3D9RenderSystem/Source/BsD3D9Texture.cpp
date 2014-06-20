#include "BsCoreThread.h"
#include "BsD3D9Texture.h"
#include "BsD3D9PixelBuffer.h"
#include "BsException.h"
#include "BsBitwise.h"
#include "BsD3D9Mappings.h"
#include "BsD3D9RenderSystem.h"
#include "BsD3D9TextureManager.h"
#include "BsD3D9Device.h"
#include "BsD3D9DeviceManager.h"
#include "BsD3D9ResourceManager.h"

namespace BansheeEngine 
{
	
    D3D9Texture::D3D9Texture()
        :Texture(), mD3DPool(D3DPOOL_MANAGED), mDynamicTextures(false),
		mHwGammaReadSupported(false), mHwGammaWriteSupported(false), mMultisampleType(D3DMULTISAMPLE_NONE),
		mMultisampleQuality(0), mIsBindableAsShaderResource(true)
	{ }
	
	D3D9Texture::~D3D9Texture()
	{	
	}

	void D3D9Texture::initialize_internal()
	{
		THROW_IF_NOT_CORE_THREAD;

		for (UINT32 i = 0; i < D3D9RenderSystem::getResourceCreationDeviceCount(); ++i)
		{
			IDirect3DDevice9* d3d9Device = D3D9RenderSystem::getResourceCreationDevice(i);

			createInternalResources(d3d9Device);
		}

		Texture::initialize_internal();
	}

	void D3D9Texture::destroy_internal()
	{
		THROW_IF_NOT_CORE_THREAD;

		for (auto& resPair : mMapDeviceToTextureResources)
		{
			TextureResources* textureResource = resPair.second;

			freeTextureResources(resPair.first, textureResource);
		}

		for (auto& resPair : mMapDeviceToTextureResources)
		{
			TextureResources* textureResource = resPair.second;

			if (textureResource != nullptr)
				bs_delete(textureResource);
		}

		mMapDeviceToTextureResources.clear();
		mSurfaceList.clear();

		clearBufferViews();

		Texture::destroy_internal();
	}

	PixelData D3D9Texture::lockImpl(GpuLockOptions options, UINT32 mipLevel, UINT32 face)
	{
		if(mLockedBuffer != nullptr)
			BS_EXCEPT(InternalErrorException, "Trying to lock a buffer that's already locked.");

		if(getUsage() == TU_DEPTHSTENCIL)
			BS_EXCEPT(InternalErrorException, "Cannot lock a depth stencil texture.");

		UINT32 mipWidth = mWidth >> mipLevel;
		UINT32 mipHeight = mHeight >> mipLevel;
		UINT32 mipDepth = mDepth >> mipLevel;

		PixelData lockedArea(mipWidth, mipHeight, mipDepth, mFormat);

		mLockedBuffer = getBuffer(face, mipLevel);
		lockedArea.setExternalBuffer((UINT8*)mLockedBuffer->lock(options));

		return lockedArea;
	}
	
	void D3D9Texture::unlockImpl()
	{
		if(mLockedBuffer == nullptr)
			BS_EXCEPT(InternalErrorException, "Trying to unlock a buffer that's not locked.");

		mLockedBuffer->unlock();
		mLockedBuffer = nullptr;
	}

	void D3D9Texture::readData(PixelData& dest, UINT32 mipLevel, UINT32 face)
	{
		PixelData myData = lock(GBL_READ_ONLY, mipLevel, face);

#if BS_DEBUG_MODE
		if(dest.getConsecutiveSize() != myData.getConsecutiveSize())
		{
			unlock();
			BS_EXCEPT(InternalErrorException, "Buffer sizes don't match.");
		}
#endif

		PixelUtil::bulkPixelConversion(myData, dest);

		unlock();
	}

	void D3D9Texture::writeData(const PixelData& src, UINT32 mipLevel, UINT32 face, bool discardWholeBuffer)
	{
		if(mUsage == TU_DYNAMIC || mUsage == TU_STATIC)
		{
			PixelData myData = lock(discardWholeBuffer ? GBL_WRITE_ONLY_DISCARD : GBL_WRITE_ONLY, mipLevel, face);
			PixelUtil::bulkPixelConversion(src, myData);
			unlock();
		}
		else
		{
			BS_EXCEPT(RenderingAPIException, "Trying to write into a buffer with unsupported usage: " + toString(mUsage));
		}
	}
	
	void D3D9Texture::copyImpl(TexturePtr& target)
	{
		THROW_IF_NOT_CORE_THREAD;

		if (target->getUsage() != getUsage() || target->getTextureType() != getTextureType())
		{
			BS_EXCEPT(InvalidParametersException, "Src. and dest. textures must be of same type and must have the same usage.");
		}

        HRESULT hr;
		D3D9Texture *other = static_cast<D3D9Texture*>(target.get());
		RECT dstRC = { 0, 0, (LONG)(other->getWidth()), (LONG)(other->getHeight()) };

		for (auto& resPair : mMapDeviceToTextureResources)
		{
			TextureResources* srcTextureResource = resPair.second;
			TextureResources* dstTextureResource = other->getTextureResources(resPair.first);

			// Plain copy for normal textures
			if (getTextureType() == TEX_TYPE_2D && srcTextureResource->pNormTex != nullptr && 
				dstTextureResource->pNormTex != nullptr)
			{			
				IDirect3DSurface9 *sourceSurface = 0;
				if(FAILED(hr = srcTextureResource->pNormTex->GetSurfaceLevel(0, &sourceSurface)))
				{
					String msg = DXGetErrorDescription(hr);
					BS_EXCEPT(RenderingAPIException, "Couldn't blit: " + msg);
				}

				IDirect3DSurface9 *destSurface = 0;			
				if(FAILED(hr = dstTextureResource->pNormTex->GetSurfaceLevel(0, &destSurface)))
				{
					String msg = DXGetErrorDescription(hr);
					SAFE_RELEASE(sourceSurface);
					BS_EXCEPT(RenderingAPIException, "Couldn't blit: " + msg);
				}

				if (FAILED(hr = resPair.first->StretchRect(sourceSurface, NULL, destSurface, &dstRC, D3DTEXF_NONE)))
				{
					String msg = DXGetErrorDescription(hr);
					SAFE_RELEASE(sourceSurface);
					SAFE_RELEASE(destSurface);
					BS_EXCEPT(RenderingAPIException, "Couldn't blit: " + msg);
				}

				SAFE_RELEASE(sourceSurface);
				SAFE_RELEASE(destSurface);
			}
			else if (getTextureType() == TEX_TYPE_CUBE_MAP && srcTextureResource->pCubeTex != nullptr && 
				dstTextureResource->pCubeTex != nullptr)
			{				
				for (UINT32 face = 0; face < 6; face++)
				{
					IDirect3DSurface9 *sourceSurface = 0;
					if( FAILED(hr = srcTextureResource->pCubeTex->GetCubeMapSurface((D3DCUBEMAP_FACES)face, 0, &sourceSurface)))
					{
						String msg = DXGetErrorDescription(hr);
						BS_EXCEPT(RenderingAPIException, "Couldn't blit: " + msg);
					}

					IDirect3DSurface9 *destSurface = 0;
					if( FAILED(hr = dstTextureResource->pCubeTex->GetCubeMapSurface((D3DCUBEMAP_FACES)face, 0, &destSurface)))
					{
						String msg = DXGetErrorDescription(hr);
						SAFE_RELEASE(sourceSurface);
						BS_EXCEPT(RenderingAPIException, "Couldn't blit: " + msg);
					}

					if (FAILED(hr = resPair.first->StretchRect(sourceSurface, NULL, destSurface, &dstRC, D3DTEXF_NONE)))
					{
						String msg = DXGetErrorDescription(hr);
						SAFE_RELEASE(sourceSurface);
						SAFE_RELEASE(destSurface);
						BS_EXCEPT(RenderingAPIException, "Couldn't blit: " + msg);
					}

					SAFE_RELEASE(sourceSurface);
					SAFE_RELEASE(destSurface);
				}
			}
			else
			{
				BS_EXCEPT(NotImplementedException, "Copy to texture is implemented only for 2D and cube textures.");
			}
		}		
	}
	
	D3D9Texture::TextureResources* D3D9Texture::getTextureResources(IDirect3DDevice9* d3d9Device)
	{		
		auto iterFind = mMapDeviceToTextureResources.find(d3d9Device);

		if (iterFind == mMapDeviceToTextureResources.end())		
			return nullptr;		
		
		return iterFind->second;
	}

	D3D9Texture::TextureResources* D3D9Texture::allocateTextureResources(IDirect3DDevice9* d3d9Device)
	{
		assert(mMapDeviceToTextureResources.find(d3d9Device) == mMapDeviceToTextureResources.end());

		TextureResources* textureResources = bs_new<TextureResources>();

		textureResources->pNormTex = nullptr;
		textureResources->pCubeTex = nullptr;
		textureResources->pVolumeTex = nullptr;
		textureResources->pBaseTex = nullptr;
		textureResources->pMultisampleSurface = nullptr;
		textureResources->pDepthStencilSurface = nullptr;

		mMapDeviceToTextureResources[d3d9Device] = textureResources;

		return textureResources;
	}
	
	void D3D9Texture::freeTextureResources(IDirect3DDevice9* d3d9Device, D3D9Texture::TextureResources* textureResources)
	{		
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		// Release surfaces from each mip level.
		for(unsigned int i = 0; i < mSurfaceList.size(); ++i)
		{
			D3D9PixelBuffer* pixelBuffer = static_cast<D3D9PixelBuffer*>(mSurfaceList[i].get());

			pixelBuffer->releaseSurfaces(d3d9Device);			
		}
		
		// Release the rest of the resources.
		SAFE_RELEASE(textureResources->pBaseTex);
		SAFE_RELEASE(textureResources->pNormTex);
		SAFE_RELEASE(textureResources->pCubeTex);
		SAFE_RELEASE(textureResources->pVolumeTex);
		SAFE_RELEASE(textureResources->pMultisampleSurface);
		SAFE_RELEASE(textureResources->pDepthStencilSurface);
	}
	
	UINT32 D3D9Texture::calculateSize() const
	{
		UINT32 instanceSize = getNumFaces() * PixelUtil::getMemorySize(mWidth, mHeight, mDepth, mFormat);

		return instanceSize * (UINT32)mMapDeviceToTextureResources.size();
	}
	
	void D3D9Texture::determinePool()
	{
		if (useDefaultPool())
			mD3DPool = D3DPOOL_DEFAULT;
		else
			mD3DPool = D3DPOOL_MANAGED;
	}
	
	void D3D9Texture::createInternalResources(IDirect3DDevice9* d3d9Device)
	{		
		switch (getTextureType())
		{
		case TEX_TYPE_1D:
		case TEX_TYPE_2D:
			createNormTex(d3d9Device);
			break;
		case TEX_TYPE_CUBE_MAP:
			createCubeTex(d3d9Device);
			break;
		case TEX_TYPE_3D:
			createVolumeTex(d3d9Device);
			break;
		default:
			destroy_internal();
			BS_EXCEPT(InternalErrorException, "Unknown texture type.");
		}
	}

	void D3D9Texture::createNormTex(IDirect3DDevice9* d3d9Device)
	{
		assert(mWidth > 0 || mHeight > 0);

		D3DFORMAT d3dPF = chooseD3DFormat(d3d9Device);
		if(mFormat != D3D9Mappings::_getPF(d3dPF))
		{
			BS_EXCEPT(RenderingAPIException, "Provided pixel format is not supported by the driver: " + toString(mFormat));
		}

		// Use D3DX to help us create the texture, this way it can adjust any relevant sizes
		UINT numMips = (mNumMipmaps == MIP_UNLIMITED) ? D3DX_DEFAULT : mNumMipmaps + 1;

		DWORD usage = 0;
		if((mUsage & TU_RENDERTARGET) != 0)
			usage = D3DUSAGE_RENDERTARGET;
		else if((mUsage & TU_DEPTHSTENCIL) != 0)
			usage = D3DUSAGE_DEPTHSTENCIL;

		// Check dynamic textures
		if (mUsage & TU_DYNAMIC)
		{
			if (canUseDynamicTextures(d3d9Device, usage, D3DRTYPE_TEXTURE, d3dPF))
			{
				usage |= D3DUSAGE_DYNAMIC;
				mDynamicTextures = true;
			}
			else
			{
				mDynamicTextures = false;
			}
		}

		// Check sRGB support
		if (mHwGamma)
		{
			mHwGammaReadSupported = canUseHardwareGammaCorrection(d3d9Device, usage, D3DRTYPE_TEXTURE, d3dPF, false);
			if (mUsage & TU_RENDERTARGET)
				mHwGammaWriteSupported = canUseHardwareGammaCorrection(d3d9Device, usage, D3DRTYPE_TEXTURE, d3dPF, true);
		}

		// Check multisample level
		if ((mUsage & TU_RENDERTARGET) != 0 || (mUsage & TU_DEPTHSTENCIL) != 0)
		{
			D3D9RenderSystem* rsys = static_cast<D3D9RenderSystem*>(BansheeEngine::RenderSystem::instancePtr());
			rsys->determineMultisampleSettings(d3d9Device, mMultisampleCount, mMultisampleHint, d3dPF, false, &mMultisampleType, &mMultisampleQuality);
		}
		else
		{
			mMultisampleType = D3DMULTISAMPLE_NONE;
			mMultisampleQuality = 0;
		}

		D3D9Device* device = D3D9RenderSystem::getDeviceManager()->getDeviceFromD3D9Device(d3d9Device);
		const D3DCAPS9& rkCurCaps = device->getD3D9DeviceCaps();			

		// Check if mip maps are supported on hardware
		if (numMips > 1 && (!(rkCurCaps.TextureCaps & D3DPTEXTURECAPS_MIPMAP) || (mUsage & TU_RENDERTARGET) != 0 || (mUsage & TU_DEPTHSTENCIL) != 0))
		{
			BS_EXCEPT(InvalidParametersException, "Invalid number of mipmaps. Maximum allowed is: 0");
		}

		determinePool();
		TextureResources* textureResources;			
	
		// Get or create new texture resources structure.
		textureResources = getTextureResources(d3d9Device);
		if (textureResources != NULL)
			freeTextureResources(d3d9Device, textureResources);
		else
			textureResources = allocateTextureResources(d3d9Device);

		if ((mUsage & TU_RENDERTARGET) != 0 && (mMultisampleType != D3DMULTISAMPLE_NONE))
		{
			// Create AA surface
			HRESULT hr = d3d9Device->CreateRenderTarget(mWidth, mHeight, d3dPF, 
				mMultisampleType, mMultisampleQuality,
				TRUE, // TODO - Possible performance issues? Need to check
				&textureResources->pMultisampleSurface, NULL);

			if (FAILED(hr))
				BS_EXCEPT(RenderingAPIException, "Unable to create AA render target: " + String(DXGetErrorDescription(hr)));

			D3DSURFACE_DESC desc;
			hr = textureResources->pMultisampleSurface->GetDesc(&desc);
			if (FAILED(hr))
			{
				destroy_internal();
				BS_EXCEPT(RenderingAPIException, "Can't get texture description: " + String(DXGetErrorDescription(hr)));
			}

			setFinalAttributes(d3d9Device, textureResources, desc.Width, desc.Height, 1, D3D9Mappings::_getPF(desc.Format));

			mIsBindableAsShaderResource = true; // Cannot bind AA surfaces
		}
		else if ((mUsage & TU_DEPTHSTENCIL) != 0 && (mMultisampleType != D3DMULTISAMPLE_NONE))
		{
			// Create AA depth stencil surface
			HRESULT hr = d3d9Device->CreateDepthStencilSurface(mWidth, mHeight, d3dPF, 
				mMultisampleType, mMultisampleQuality,
				TRUE, // TODO - Possible performance issues? Need to check
				&textureResources->pDepthStencilSurface, NULL);

			if (FAILED(hr))
				BS_EXCEPT(RenderingAPIException, "Unable to create AA depth stencil render target: " + String(DXGetErrorDescription(hr)));

			// Update final parameters as they may differ from requested ones
			D3DSURFACE_DESC desc;
			hr = textureResources->pDepthStencilSurface->GetDesc(&desc);
			if (FAILED(hr))
			{
				destroy_internal();
				BS_EXCEPT(RenderingAPIException, "Can't get texture description: " + String(DXGetErrorDescription(hr)));
			}

			setFinalAttributes(d3d9Device, textureResources, desc.Width, desc.Height, 1, D3D9Mappings::_getPF(desc.Format));

			mIsBindableAsShaderResource = true; // Cannot bind AA depth buffer
		}
		else
		{
			// Create normal texture
			HRESULT hr = D3DXCreateTexture(d3d9Device, (UINT)mWidth, (UINT)mHeight,	numMips,
				usage, d3dPF, mD3DPool, &textureResources->pNormTex);

			if (FAILED(hr))
			{
				destroy_internal();
				BS_EXCEPT(RenderingAPIException, "Error creating texture: " + String(DXGetErrorDescription(hr)));
			}

			hr = textureResources->pNormTex->QueryInterface(IID_IDirect3DBaseTexture9, (void **)&textureResources->pBaseTex);
			if (FAILED(hr))
			{
				destroy_internal();
				BS_EXCEPT(RenderingAPIException, "Can't get base texture: " + String(DXGetErrorDescription(hr)));
			}

			// Update final parameters as they may differ from requested ones
			D3DSURFACE_DESC desc;
			hr = textureResources->pNormTex->GetLevelDesc(0, &desc);
			if (FAILED(hr))
			{
				destroy_internal();
				BS_EXCEPT(RenderingAPIException, "Can't get texture description: " + String(DXGetErrorDescription(hr)));
			}

			setFinalAttributes(d3d9Device, textureResources, desc.Width, desc.Height, 1, D3D9Mappings::_getPF(desc.Format));
		}
	}
	
	void D3D9Texture::createCubeTex(IDirect3DDevice9* d3d9Device)
	{
		assert(mWidth > 0 || mHeight > 0);

		if (mUsage & TU_RENDERTARGET)
			BS_EXCEPT(RenderingAPIException, "D3D9 Cube texture can not be created as render target !!");

		if (mUsage & TU_DEPTHSTENCIL)
			BS_EXCEPT(RenderingAPIException, "D3D9 Cube texture can not be created as a depth stencil target !!");

		D3DFORMAT d3dPF = chooseD3DFormat(d3d9Device);
		if(mFormat != D3D9Mappings::_getPF(d3dPF))
		{
			BS_EXCEPT(RenderingAPIException, "Provided pixel format is not supported by the driver: " + toString(mFormat));
		}

		// Use D3DX to help us create the texture, this way it can adjust any relevant sizes
		DWORD usage = (mUsage & TU_RENDERTARGET) ? D3DUSAGE_RENDERTARGET : 0;
		UINT numMips = (mNumMipmaps == MIP_UNLIMITED) ? D3DX_DEFAULT : mNumMipmaps + 1;

		// Check dynamic textures
		if (mUsage & TU_DYNAMIC)
		{
			if (canUseDynamicTextures(d3d9Device, usage, D3DRTYPE_CUBETEXTURE, d3dPF))
			{
				usage |= D3DUSAGE_DYNAMIC;
				mDynamicTextures = true;
			}
			else
			{
				mDynamicTextures = false;
			}
		}

		// Check sRGB support
		if (mHwGamma)
		{
			mHwGammaReadSupported = canUseHardwareGammaCorrection(d3d9Device, usage, D3DRTYPE_CUBETEXTURE, d3dPF, false);
			if (mUsage & TU_RENDERTARGET)
				mHwGammaWriteSupported = canUseHardwareGammaCorrection(d3d9Device, usage, D3DRTYPE_CUBETEXTURE, d3dPF, true);
		}

		// No multisampling on cube textures
		mMultisampleType = D3DMULTISAMPLE_NONE;
		mMultisampleQuality = 0;

		D3D9Device* device = D3D9RenderSystem::getDeviceManager()->getDeviceFromD3D9Device(d3d9Device);
		const D3DCAPS9& deviceCaps = device->getD3D9DeviceCaps();			
		
		// Check if mip map cube textures are supported
		if (numMips > 1 && !(deviceCaps.TextureCaps & D3DPTEXTURECAPS_MIPCUBEMAP))
		{
			BS_EXCEPT(InvalidParametersException, "Invalid number of mipmaps. Maximum allowed is: 0");
		}

		determinePool();
		TextureResources* textureResources;			

		// Get or create new texture resources structure.
		textureResources = getTextureResources(d3d9Device);
		if (textureResources != NULL)
			freeTextureResources(d3d9Device, textureResources);
		else
			textureResources = allocateTextureResources(d3d9Device);

		// Create the texture
		HRESULT hr = D3DXCreateCubeTexture(d3d9Device, (UINT)mWidth, numMips,
				usage, d3dPF, mD3DPool, &textureResources->pCubeTex);

		if (FAILED(hr))
		{
			destroy_internal();
			BS_EXCEPT(RenderingAPIException, "Error creating texture: " + String(DXGetErrorDescription(hr)));
		}

		hr = textureResources->pCubeTex->QueryInterface(IID_IDirect3DBaseTexture9, (void **)&textureResources->pBaseTex);
		if (FAILED(hr))
		{
			destroy_internal();
			BS_EXCEPT(RenderingAPIException, "Can't get base texture: " + String(DXGetErrorDescription(hr)));
		}
		
		// Update final parameters as they may differ from requested ones
		D3DSURFACE_DESC desc;
		hr = textureResources->pCubeTex->GetLevelDesc(0, &desc);
		if (FAILED(hr))
		{
			destroy_internal();
			BS_EXCEPT(RenderingAPIException, "Can't get texture description: " + String(DXGetErrorDescription(hr)));
		}

		setFinalAttributes(d3d9Device, textureResources, desc.Width, desc.Height, 1, D3D9Mappings::_getPF(desc.Format));		
	}
	
	void D3D9Texture::createVolumeTex(IDirect3DDevice9* d3d9Device)
	{
		assert(mWidth > 0 && mHeight > 0 && mDepth>0);

		if (mUsage & TU_RENDERTARGET)
			BS_EXCEPT(RenderingAPIException, "D3D9 Volume texture can not be created as render target !!");

		if (mUsage & TU_DEPTHSTENCIL)
			BS_EXCEPT(RenderingAPIException, "D3D9 Volume texture can not be created as a depth stencil target !!");

		D3DFORMAT d3dPF = chooseD3DFormat(d3d9Device);
		if(mFormat != D3D9Mappings::_getPF(d3dPF))
		{
			BS_EXCEPT(RenderingAPIException, "Provided pixel format is not supported by the driver: " + toString(mFormat));
		}

		// Use D3DX to help us create the texture, this way it can adjust any relevant sizes
		DWORD usage = (mUsage & TU_RENDERTARGET) ? D3DUSAGE_RENDERTARGET : 0;
		UINT numMips = (mNumMipmaps == MIP_UNLIMITED) ? D3DX_DEFAULT : mNumMipmaps + 1;

		// Check dynamic textures
		if (mUsage & TU_DYNAMIC)
		{
			if (canUseDynamicTextures(d3d9Device, usage, D3DRTYPE_VOLUMETEXTURE, d3dPF))
			{
				usage |= D3DUSAGE_DYNAMIC;
				mDynamicTextures = true;
			}
			else
			{
				mDynamicTextures = false;
			}
		}

		// Check sRGB support
		if (mHwGamma)
		{
			mHwGammaReadSupported = canUseHardwareGammaCorrection(d3d9Device, usage, D3DRTYPE_VOLUMETEXTURE, d3dPF, false);
			if (mUsage & TU_RENDERTARGET)
				mHwGammaWriteSupported = canUseHardwareGammaCorrection(d3d9Device, usage, D3DRTYPE_VOLUMETEXTURE, d3dPF, true);
		}

		D3D9Device* device = D3D9RenderSystem::getDeviceManager()->getDeviceFromD3D9Device(d3d9Device);
		const D3DCAPS9& rkCurCaps = device->getD3D9DeviceCaps();			

		// Check if mip map volume textures are supported
		if (numMips > 1 && !(rkCurCaps.TextureCaps & D3DPTEXTURECAPS_MIPVOLUMEMAP))
		{
			BS_EXCEPT(InvalidParametersException, "Invalid number of mipmaps. Maximum allowed is: 0");
		}

		determinePool();
		TextureResources* textureResources;			

		// Get or create new texture resources structure.
		textureResources = getTextureResources(d3d9Device);
		if (textureResources != NULL)
			freeTextureResources(d3d9Device, textureResources);
		else
			textureResources = allocateTextureResources(d3d9Device);

		// Create the texture
		HRESULT hr = D3DXCreateVolumeTexture(d3d9Device, (UINT)mWidth, (UINT)mHeight, (UINT)mDepth,
				numMips, usage, d3dPF, mD3DPool, &textureResources->pVolumeTex);

		if (FAILED(hr))
		{
			destroy_internal();
			BS_EXCEPT(RenderingAPIException, "Error creating texture: " + String(DXGetErrorDescription(hr)));
		}

		hr = textureResources->pVolumeTex->QueryInterface(IID_IDirect3DBaseTexture9, (void**)&textureResources->pBaseTex);
		if (FAILED(hr))
		{
			destroy_internal();
			BS_EXCEPT(RenderingAPIException, "Can't get base texture: " + String(DXGetErrorDescription(hr)));
		}
		
		// Update final parameters as they may differ from requested ones
		D3DVOLUME_DESC desc;
		hr = textureResources->pVolumeTex->GetLevelDesc(0, &desc);
		if (FAILED(hr))
		{
			destroy_internal();
			BS_EXCEPT(RenderingAPIException, "Can't get texture description: " + String(DXGetErrorDescription(hr)));
		}

		setFinalAttributes(d3d9Device, textureResources, desc.Width, desc.Height, desc.Depth, D3D9Mappings::_getPF(desc.Format));
	}

	void D3D9Texture::setFinalAttributes(IDirect3DDevice9* d3d9Device, TextureResources* textureResources,
		UINT32 width, UINT32 height, UINT32 depth, PixelFormat format)
	{ 
		if(width != mWidth || height != mHeight || depth != mDepth)
		{
			BS_EXCEPT(InternalErrorException, "Wanted and created textures sizes don't match!" \
				"Width: " + toString(width) + "/" + toString(mWidth) +
				"Height: " + toString(height) + "/" + toString(mHeight) +
				"Depth: " + toString(depth) + "/" + toString(mDepth));
		}

		if(format != mFormat)
		{
			BS_EXCEPT(InternalErrorException, "Wanted and created texture formats don't match! " + toString(format) + "/" + toString(mFormat));
		}
		
		createSurfaceList(d3d9Device, textureResources);
	}
	
	D3DTEXTUREFILTERTYPE D3D9Texture::getBestFilterMethod(IDirect3DDevice9* d3d9Device)
	{
		D3D9Device* device = D3D9RenderSystem::getDeviceManager()->getDeviceFromD3D9Device(d3d9Device);
		const D3DCAPS9& deviceCaps = device->getD3D9DeviceCaps();			
		
		DWORD filterCaps = 0;

		switch (getTextureType())
		{
		case TEX_TYPE_1D:		
			// Same as 2D
		case TEX_TYPE_2D:		
			filterCaps = deviceCaps.TextureFilterCaps;	
			break;
		case TEX_TYPE_3D:		
			filterCaps = deviceCaps.VolumeTextureFilterCaps;	
			break;
		case TEX_TYPE_CUBE_MAP:	
			filterCaps = deviceCaps.CubeTextureFilterCaps;	
			break;
		}

		if(filterCaps & D3DPTFILTERCAPS_MINFGAUSSIANQUAD)
			return D3DTEXF_GAUSSIANQUAD;
		
		if(filterCaps & D3DPTFILTERCAPS_MINFPYRAMIDALQUAD)
			return D3DTEXF_PYRAMIDALQUAD;
		
		if(filterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC)
			return D3DTEXF_ANISOTROPIC;
		
		if(filterCaps & D3DPTFILTERCAPS_MINFLINEAR)
			return D3DTEXF_LINEAR;
		
		if(filterCaps & D3DPTFILTERCAPS_MINFPOINT)
			return D3DTEXF_POINT;
		
		return D3DTEXF_POINT;
	}
	
	bool D3D9Texture::canUseDynamicTextures(IDirect3DDevice9* d3d9Device, DWORD srcUsage, D3DRESOURCETYPE srcType, 
		D3DFORMAT srcFormat)
	{		
		IDirect3D9* pD3D = nullptr;

		HRESULT hr = d3d9Device->GetDirect3D(&pD3D);
		if (FAILED(hr))
			BS_EXCEPT(InvalidParametersException, "GetDirect3D failed !!!");

		if (pD3D != nullptr)
			pD3D->Release();
	
		D3D9Device* device = D3D9RenderSystem::getDeviceManager()->getDeviceFromD3D9Device(d3d9Device);
		const D3DCAPS9& deviceCaps = device->getD3D9DeviceCaps();						
		D3DFORMAT backBufferFormat = device->getBackBufferFormat();

		hr = pD3D->CheckDeviceFormat(deviceCaps.AdapterOrdinal, deviceCaps.DeviceType, 
			backBufferFormat, srcUsage | D3DUSAGE_DYNAMIC, srcType, srcFormat);

		return hr == D3D_OK;
	}
	
	bool D3D9Texture::canUseHardwareGammaCorrection(IDirect3DDevice9* d3d9Device, DWORD srcUsage, 
		D3DRESOURCETYPE srcType, D3DFORMAT srcFormat, bool forwriting)
	{
		IDirect3D9* pD3D = nullptr;

		HRESULT hr = d3d9Device->GetDirect3D(&pD3D);
		if (FAILED(hr))
		{
			BS_EXCEPT(InvalidParametersException, "GetDirect3D failed !!!" );
		}

		if (pD3D != nullptr)
			pD3D->Release();

		D3D9Device* device = D3D9RenderSystem::getDeviceManager()->getDeviceFromD3D9Device(d3d9Device);
		const D3DCAPS9& deviceCaps = device->getD3D9DeviceCaps();						
		D3DFORMAT backBufferFormat = device->getBackBufferFormat();

		if (forwriting)
			srcUsage |= D3DUSAGE_QUERY_SRGBWRITE;
		else
			srcUsage |= D3DUSAGE_QUERY_SRGBREAD;

		hr = pD3D->CheckDeviceFormat(deviceCaps.AdapterOrdinal, deviceCaps.DeviceType, 
			backBufferFormat, srcUsage, srcType, srcFormat);

		return hr == D3D_OK;
	}
	
	bool D3D9Texture::canAutoGenMipmaps(IDirect3DDevice9* d3d9Device, DWORD srcUsage, D3DRESOURCETYPE srcType, D3DFORMAT srcFormat)
	{
		IDirect3D9* pD3D = nullptr;

		HRESULT hr = d3d9Device->GetDirect3D(&pD3D);
		if (FAILED(hr))
		{
			BS_EXCEPT(InvalidParametersException, "GetDirect3D failed.");
		}

		if (pD3D != nullptr)
			pD3D->Release();

		D3D9Device* device = D3D9RenderSystem::getDeviceManager()->getDeviceFromD3D9Device(d3d9Device);
		const D3DCAPS9& deviceCaps = device->getD3D9DeviceCaps();						
		D3DFORMAT backBufferFormat = device->getBackBufferFormat();

		if (deviceCaps.Caps2 & D3DCAPS2_CANAUTOGENMIPMAP)
		{
			hr = pD3D->CheckDeviceFormat(deviceCaps.AdapterOrdinal, deviceCaps.DeviceType, 
					backBufferFormat, srcUsage | D3DUSAGE_AUTOGENMIPMAP, srcType, srcFormat);
			
			return hr == D3D_OK;
		}
		else
			return false;
	}
	
	D3DFORMAT D3D9Texture::chooseD3DFormat(IDirect3DDevice9* d3d9Device)
	{		
		// Choose frame buffer pixel format in case PF_UNKNOWN was requested
		if(mFormat == PF_UNKNOWN)
		{	
			D3D9Device* device = D3D9RenderSystem::getDeviceManager()->getDeviceFromD3D9Device(d3d9Device);
			
			if((mUsage & TU_DEPTHSTENCIL) != 0)
				return device->getDepthStencilFormat();
			else
				return device->getBackBufferFormat();		
		}

		// Choose closest supported D3D format as a D3D format
		return D3D9Mappings::_getPF(D3D9Mappings::_getClosestSupportedPF(mFormat));
	}
	
	void D3D9Texture::createSurfaceList(IDirect3DDevice9* d3d9Device, TextureResources* textureResources)
	{
		assert(textureResources != nullptr);

		// Need to know static / dynamic
		UINT32 usage;
		if ((mUsage & TU_DYNAMIC) && mDynamicTextures)
		{
			usage = GBU_DYNAMIC;
		}
		else
		{
			usage = GBU_STATIC;
		}

		if (mUsage & TU_RENDERTARGET)
		{
			usage |= TU_RENDERTARGET;
		}
		else if(mUsage & TU_DEPTHSTENCIL)
		{
			usage |= TU_RENDERTARGET;
		}
		
		UINT32 surfaceCount  = static_cast<UINT32>((getNumFaces() * (mNumMipmaps + 1)));
		bool updateOldList = mSurfaceList.size() == surfaceCount;
		if(!updateOldList)
		{			
			mSurfaceList.clear();
			for(UINT32 face = 0; face < getNumFaces(); face++)
			{
				for(UINT32 mip = 0; mip <= mNumMipmaps; mip++)
				{
					mSurfaceList.push_back(bs_shared_ptr<D3D9PixelBuffer>((GpuBufferUsage)usage, this));
				}
			}
		}

		IDirect3DSurface9* surface = nullptr;
		IDirect3DVolume9* volume = nullptr;

		if((mUsage & TU_RENDERTARGET) != 0 && (mMultisampleType != D3DMULTISAMPLE_NONE))
		{
			assert(textureResources->pMultisampleSurface);
			assert(getTextureType() == TEX_TYPE_2D);

			D3D9PixelBuffer* currPixelBuffer = static_cast<D3D9PixelBuffer*>(mSurfaceList[0].get());

			currPixelBuffer->bind(d3d9Device, textureResources->pMultisampleSurface, textureResources->pBaseTex);
		}
		else if((mUsage & TU_DEPTHSTENCIL) != 0 && (mMultisampleType != D3DMULTISAMPLE_NONE))
		{
			assert(textureResources->pDepthStencilSurface);
			assert(getTextureType() == TEX_TYPE_2D);

			D3D9PixelBuffer* currPixelBuffer = static_cast<D3D9PixelBuffer*>(mSurfaceList[0].get());

			currPixelBuffer->bind(d3d9Device, textureResources->pDepthStencilSurface, textureResources->pBaseTex);
		}
		else
		{
			assert(textureResources->pBaseTex);

			UINT32 numCreatedMips = textureResources->pBaseTex->GetLevelCount() - 1;
			if(numCreatedMips != mNumMipmaps)
			{
				BS_EXCEPT(InternalErrorException, "Number of created and wanted mip map levels doesn't match: " + 
					toString(numCreatedMips) + "/" + toString(mNumMipmaps));
			}

			switch(getTextureType()) 
			{
			case TEX_TYPE_2D:
			case TEX_TYPE_1D:
				assert(textureResources->pNormTex);

				for(UINT32 mip = 0; mip <= mNumMipmaps; mip++)
				{
					if(textureResources->pNormTex->GetSurfaceLevel(static_cast<UINT>(mip), &surface) != D3D_OK)
						BS_EXCEPT(RenderingAPIException, "Get surface level failed");

					D3D9PixelBuffer* currPixelBuffer = static_cast<D3D9PixelBuffer*>(mSurfaceList[mip].get());

					currPixelBuffer->bind(d3d9Device, surface, textureResources->pBaseTex);

					surface->Release();			
				}

				break;
			case TEX_TYPE_CUBE_MAP:
				assert(textureResources->pCubeTex);

				for(UINT32 face = 0; face < 6; face++)
				{
					for(UINT32 mip = 0; mip <= mNumMipmaps; mip++)
					{
						if(textureResources->pCubeTex->GetCubeMapSurface((D3DCUBEMAP_FACES)face, static_cast<UINT>(mip), &surface) != D3D_OK)
							BS_EXCEPT(RenderingAPIException, "Get cubemap surface failed");

						UINT32 idx = face*(mNumMipmaps + 1) + mip;
						D3D9PixelBuffer* currPixelBuffer = static_cast<D3D9PixelBuffer*>(mSurfaceList[idx].get());

						currPixelBuffer->bind(d3d9Device, surface, textureResources->pBaseTex);

						surface->Release();				
					}				
				}
				break;
			case TEX_TYPE_3D:
				assert(textureResources->pVolumeTex);

				for(UINT32 mip = 0; mip <= mNumMipmaps; mip++)
				{
					if(textureResources->pVolumeTex->GetVolumeLevel(static_cast<UINT>(mip), &volume) != D3D_OK)
						BS_EXCEPT(RenderingAPIException, "Get volume level failed");	

					D3D9PixelBuffer* currPixelBuffer = static_cast<D3D9PixelBuffer*>(mSurfaceList[mip].get());
					currPixelBuffer->bind(d3d9Device, volume, textureResources->pBaseTex);

					volume->Release();
				}
				break;
			};		
		}		
	}

	PixelBufferPtr D3D9Texture::getBuffer(UINT32 face, UINT32 mipmap) 
	{
		THROW_IF_NOT_CORE_THREAD;

		if(face >= getNumFaces())
			BS_EXCEPT(InvalidParametersException, "A three dimensional cube has six faces");
		if(mipmap > mNumMipmaps)
			BS_EXCEPT(InvalidParametersException, "Mipmap index out of range");

		UINT32 idx = face*(mNumMipmaps+1) + mipmap;

		IDirect3DDevice9* d3d9Device = D3D9RenderSystem::getActiveD3D9Device();
		TextureResources* textureResources = getTextureResources(d3d9Device);
		if (textureResources == nullptr || textureResources->pBaseTex == nullptr)
		{				
			createInternalResources(d3d9Device);
			textureResources = getTextureResources(d3d9Device);			
		}
	
		assert(textureResources != nullptr);
		assert(idx < mSurfaceList.size());
		return mSurfaceList[idx];
	}

	bool D3D9Texture::useDefaultPool()
	{
		return (mUsage & TU_RENDERTARGET) || (mUsage & TU_DEPTHSTENCIL) || ((mUsage & TU_DYNAMIC) && mDynamicTextures);
	}
	
	void D3D9Texture::notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device) 
	{		
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		if (D3D9RenderSystem::getResourceManager()->getCreationPolicy() == RCP_CREATE_ON_ALL_DEVICES)
			createInternalResources(d3d9Device);
	}

	void D3D9Texture::notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device) 
	{				
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		auto iterFind = mMapDeviceToTextureResources.find(d3d9Device);

		if (iterFind != mMapDeviceToTextureResources.end())
		{			
			TextureResources* textureResource = iterFind->second;

			for(unsigned int i = 0; i < mSurfaceList.size(); ++i)
			{
				D3D9PixelBuffer* pixelBuffer = static_cast<D3D9PixelBuffer*>(mSurfaceList[i].get());

				pixelBuffer->destroyBufferResources(d3d9Device);			
			}

			freeTextureResources(d3d9Device, textureResource);

			if(textureResource != nullptr)
				bs_delete(textureResource);

			mMapDeviceToTextureResources.erase(iterFind);
		}	
	}

	void D3D9Texture::notifyOnDeviceLost(IDirect3DDevice9* d3d9Device) 
	{		
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		if(mD3DPool == D3DPOOL_DEFAULT)
		{
			auto iterFind = mMapDeviceToTextureResources.find(d3d9Device);

			if (iterFind != mMapDeviceToTextureResources.end())
			{
				TextureResources* textureResource = iterFind->second;				
				freeTextureResources(d3d9Device, textureResource);
			}					
		}		
	}

	void D3D9Texture::notifyOnDeviceReset(IDirect3DDevice9* d3d9Device) 
	{		
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		if(mD3DPool == D3DPOOL_DEFAULT)
		{			
			createInternalResources(d3d9Device);
		}
	}

	IDirect3DBaseTexture9* D3D9Texture::getTexture_internal()
	{
		THROW_IF_NOT_CORE_THREAD;

		TextureResources* textureResources;			
		IDirect3DDevice9* d3d9Device = D3D9RenderSystem::getActiveD3D9Device();
			
		textureResources = getTextureResources(d3d9Device);		
		if (textureResources == nullptr || textureResources->pBaseTex == nullptr)
		{			
			createInternalResources(d3d9Device);
			textureResources = getTextureResources(d3d9Device);			
		}

		assert(textureResources); 
		assert(textureResources->pBaseTex); 

		return textureResources->pBaseTex;
	}

	IDirect3DTexture9* D3D9Texture::getNormTexture_internal()
	{
		THROW_IF_NOT_CORE_THREAD;

		TextureResources* textureResources;
		IDirect3DDevice9* d3d9Device = D3D9RenderSystem::getActiveD3D9Device();
		
		textureResources = getTextureResources(d3d9Device);		
		if (textureResources == nullptr || textureResources->pNormTex == nullptr)
		{
			createInternalResources(d3d9Device);
			textureResources = getTextureResources(d3d9Device);			
		}
		assert(textureResources); 
		assert(textureResources->pNormTex); 

		return textureResources->pNormTex;
	}

	IDirect3DCubeTexture9* D3D9Texture::getCubeTexture_internal()
	{
		THROW_IF_NOT_CORE_THREAD;

		TextureResources* textureResources;
		IDirect3DDevice9* d3d9Device = D3D9RenderSystem::getActiveD3D9Device();
		
		textureResources = getTextureResources(d3d9Device);		
		if (textureResources == nullptr || textureResources->pCubeTex)
		{
			createInternalResources(d3d9Device);
			textureResources = getTextureResources(d3d9Device);			
		}

		assert(textureResources); 
		assert(textureResources->pCubeTex); 

		return textureResources->pCubeTex;
	}	
}
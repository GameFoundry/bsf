//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsCoreThread.h"
#include "BsD3D9Texture.h"
#include "BsD3D9PixelBuffer.h"
#include "BsException.h"
#include "BsBitwise.h"
#include "BsD3D9Mappings.h"
#include "BsD3D9RenderAPI.h"
#include "BsD3D9TextureManager.h"
#include "BsD3D9Device.h"
#include "BsD3D9DeviceManager.h"
#include "BsD3D9ResourceManager.h"
#include "BsRenderStats.h"
#include "BsPixelData.h"

namespace BansheeEngine 
{
	D3D9TextureCore::D3D9TextureCore(TextureType textureType, UINT32 width, UINT32 height, UINT32 depth, UINT32 numMipmaps,
		PixelFormat format, int usage, bool hwGamma, UINT32 multisampleCount, const PixelDataPtr& initialData)
		:TextureCore(textureType, width, height, depth, numMipmaps, format, usage, hwGamma, multisampleCount, initialData),
		mD3DPool(D3DPOOL_MANAGED), mDynamicTextures(false), mHwGammaReadSupported(false), mHwGammaWriteSupported(false), 
		mMultisampleType(D3DMULTISAMPLE_NONE), mMultisampleQuality(0), mIsBindableAsShaderResource(true)
	{

	}
	
	D3D9TextureCore::~D3D9TextureCore()
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

		BS_INC_RENDER_STAT_CAT(ResDestroyed, RenderStatObject_Texture);
	}

	void D3D9TextureCore::initialize()
	{
		THROW_IF_NOT_CORE_THREAD;

		for (UINT32 i = 0; i < D3D9RenderAPI::getResourceCreationDeviceCount(); ++i)
		{
			IDirect3DDevice9* d3d9Device = D3D9RenderAPI::getResourceCreationDevice(i);

			createInternalResources(d3d9Device);
		}

		BS_INC_RENDER_STAT_CAT(ResCreated, RenderStatObject_Texture);
		TextureCore::initialize();
	}

	PixelData D3D9TextureCore::lockImpl(GpuLockOptions options, UINT32 mipLevel, UINT32 face)
	{
		if (mProperties.getMultisampleCount() > 1)
			BS_EXCEPT(InvalidStateException, "Multisampled textures cannot be accessed from the CPU directly.");

		if(mLockedBuffer != nullptr)
			BS_EXCEPT(InternalErrorException, "Trying to lock a buffer that's already locked.");

		if (mProperties.getUsage() == TU_DEPTHSTENCIL)
			BS_EXCEPT(InternalErrorException, "Cannot lock a depth stencil texture.");

		UINT32 mipWidth = std::max(1u, mProperties.getWidth() >> mipLevel);
		UINT32 mipHeight = std::max(1u, mProperties.getHeight() >> mipLevel);
		UINT32 mipDepth = std::max(1u, mProperties.getDepth() >> mipLevel);

		PixelData lockedArea(mipWidth, mipHeight, mipDepth, mProperties.getFormat());

		mLockedBuffer = getBuffer(face, mipLevel);
		lockedArea.setExternalBuffer((UINT8*)mLockedBuffer->lock(options));

		return lockedArea;
	}
	
	void D3D9TextureCore::unlockImpl()
	{
		if(mLockedBuffer == nullptr)
			BS_EXCEPT(InternalErrorException, "Trying to unlock a buffer that's not locked.");

		mLockedBuffer->unlock();
		mLockedBuffer = nullptr;
	}

	void D3D9TextureCore::readData(PixelData& dest, UINT32 mipLevel, UINT32 face)
	{
		if (mProperties.getMultisampleCount() > 1)
			BS_EXCEPT(InvalidStateException, "Multisampled textures cannot be accessed from the CPU directly.");

		if (mProperties.getUsage() == TU_DEPTHSTENCIL || mProperties.getUsage() == TU_RENDERTARGET) // Render targets cannot be locked normally
		{
			IDirect3DDevice9* device = D3D9RenderAPI::getActiveD3D9Device();

			D3D9PixelBuffer* sourceBuffer = static_cast<D3D9PixelBuffer*>(getBuffer(face, mipLevel).get());
			
			UINT32 mipWidth = std::max(1u, mProperties.getWidth() >> mipLevel);
			UINT32 mipHeight = std::max(1u, mProperties.getHeight() >> mipLevel);
			D3DFORMAT format = chooseD3DFormat(device);

			// Note: I'm allocating and releasing a texture every time we read.
			// I might consider adding a flag to keep this texture permanent which would make reading
			// faster but at the cost of double the memory.

			IDirect3DTexture9* stagingTexture = nullptr; 
			HRESULT hr = D3DXCreateTexture(device, (UINT)mipWidth, (UINT)mipHeight, 1,
				0, format, D3DPOOL_SYSTEMMEM, &stagingTexture);

			if (FAILED(hr))
			{
				String msg = DXGetErrorDescription(hr);
				BS_EXCEPT(RenderingAPIException, "Failed to create a texture: " + msg);
			}

			IDirect3DSurface9* stagingSurface = nullptr;
			hr = stagingTexture->GetSurfaceLevel(0, &stagingSurface);
			if (FAILED(hr))
			{
				String msg = DXGetErrorDescription(hr);
				BS_EXCEPT(RenderingAPIException, "Failed to retrieve a texture surface: " + msg);
			}

			hr = device->GetRenderTargetData(sourceBuffer->getSurface(device), stagingSurface);
			if (FAILED(hr))
			{
				String msg = DXGetErrorDescription(hr);
				BS_EXCEPT(RenderingAPIException, "Failed to retrieve render target data: " + msg);
			}

			PixelData myData(mipWidth, mipHeight, 1, mProperties.getFormat());

			D3DLOCKED_RECT lrect;
			hr = stagingSurface->LockRect(&lrect, nullptr, D3DLOCK_READONLY);
			if (FAILED(hr))
			{
				String msg = DXGetErrorDescription(hr);
				BS_EXCEPT(RenderingAPIException, "Failed to lock surface: " + msg);
			}

			D3D9PixelBuffer::initPixelDataFromD3DLock(myData, lrect);

#if BS_DEBUG_MODE
			if (dest.getConsecutiveSize() != myData.getConsecutiveSize())
				BS_EXCEPT(InternalErrorException, "Buffer sizes don't match.");
#endif

			PixelUtil::bulkPixelConversion(myData, dest);

			hr = stagingSurface->UnlockRect();
			if (FAILED(hr))
			{
				String msg = DXGetErrorDescription(hr);
				BS_EXCEPT(RenderingAPIException, "Failed to unlock surface: " + msg);
			}

			SAFE_RELEASE(stagingSurface);
			SAFE_RELEASE(stagingTexture);
		}
		else
		{
			PixelData myData = lock(GBL_READ_ONLY, mipLevel, face);

#if BS_DEBUG_MODE
			if (dest.getConsecutiveSize() != myData.getConsecutiveSize())
			{
				unlock();
				BS_EXCEPT(InternalErrorException, "Buffer sizes don't match.");
			}
#endif

			PixelUtil::bulkPixelConversion(myData, dest);

			unlock();
		}
	}

	void D3D9TextureCore::writeData(const PixelData& src, UINT32 mipLevel, UINT32 face, bool discardWholeBuffer)
	{
		if (mProperties.getMultisampleCount() > 1)
			BS_EXCEPT(InvalidStateException, "Multisampled textures cannot be accessed from the CPU directly.");

		if (mProperties.getUsage() == TU_DYNAMIC || mProperties.getUsage() == TU_STATIC)
		{
			PixelData myData = lock(discardWholeBuffer ? GBL_WRITE_ONLY_DISCARD : GBL_WRITE_ONLY, mipLevel, face);
			PixelUtil::bulkPixelConversion(src, myData);
			unlock();
		}
		else
		{
			BS_EXCEPT(RenderingAPIException, "Trying to write into a buffer with unsupported usage: " + toString(mProperties.getUsage()));
		}
	}
	
	void D3D9TextureCore::copyImpl(UINT32 srcFace, UINT32 srcMipLevel, UINT32 destFace, UINT32 destMipLevel, const SPtr<TextureCore>& target)
	{
		THROW_IF_NOT_CORE_THREAD;

		TextureType texType = mProperties.getTextureType();

		if (texType == TEX_TYPE_1D || texType == TEX_TYPE_3D)
			BS_EXCEPT(NotImplementedException, "Copy not implemented for 1D and 3D textures yet.");

        HRESULT hr;
		D3D9TextureCore* other = static_cast<D3D9TextureCore*>(target.get());

		for (auto& resPair : mMapDeviceToTextureResources)
		{
			TextureResources* srcTextureResource = resPair.second;
			TextureResources* dstTextureResource = other->getTextureResources(resPair.first);

			IDirect3DSurface9 *sourceSurface = nullptr;
			IDirect3DSurface9 *destSurface = nullptr;

			if (texType == TEX_TYPE_2D && srcTextureResource->pNormTex != nullptr && dstTextureResource->pNormTex != nullptr)
			{			
				if(FAILED(hr = srcTextureResource->pNormTex->GetSurfaceLevel(srcMipLevel, &sourceSurface)))
				{
					String msg = DXGetErrorDescription(hr);
					BS_EXCEPT(RenderingAPIException, "Cannot retrieve source surface for copy: " + msg);
				}

				if(FAILED(hr = dstTextureResource->pNormTex->GetSurfaceLevel(destMipLevel, &destSurface)))
				{
					String msg = DXGetErrorDescription(hr);
					BS_EXCEPT(RenderingAPIException, "Cannot retrieve destination surface for copy: " + msg);
				}
			}
			else if (texType == TEX_TYPE_CUBE_MAP && srcTextureResource->pCubeTex != nullptr && dstTextureResource->pCubeTex != nullptr)
			{				
				IDirect3DSurface9 *sourceSurface = nullptr;
				if (FAILED(hr = srcTextureResource->pCubeTex->GetCubeMapSurface((D3DCUBEMAP_FACES)srcFace, srcMipLevel, &sourceSurface)))
				{
					String msg = DXGetErrorDescription(hr);
					BS_EXCEPT(RenderingAPIException, "Cannot retrieve source surface for copy: " + msg);
				}

				IDirect3DSurface9 *destSurface = nullptr;
				if (FAILED(hr = dstTextureResource->pCubeTex->GetCubeMapSurface((D3DCUBEMAP_FACES)destFace, destMipLevel, &destSurface)))
				{
					String msg = DXGetErrorDescription(hr);
					BS_EXCEPT(RenderingAPIException, "Cannot retrieve destination surface for copy: " + msg);
				}
			}

			if (sourceSurface != nullptr && destSurface != nullptr)
			{
				D3DSURFACE_DESC desc;

				sourceSurface->GetDesc(&desc);
				if (desc.Pool != D3DPOOL_DEFAULT)
					BS_EXCEPT(InvalidStateException, "Source surface must be in the default pool.");

				destSurface->GetDesc(&desc);
				if (desc.Pool != D3DPOOL_DEFAULT)
					BS_EXCEPT(InvalidStateException, "Destination surface must be in the default pool.");

				if (FAILED(hr = resPair.first->StretchRect(sourceSurface, NULL, destSurface, NULL, D3DTEXF_NONE)))
				{
					String msg = DXGetErrorDescription(hr);
					BS_EXCEPT(RenderingAPIException, "StretchRect failed during copy: " + msg);
				}
			}

			SAFE_RELEASE(sourceSurface);
			SAFE_RELEASE(destSurface);
		}		
	}
	
	D3D9TextureCore::TextureResources* D3D9TextureCore::getTextureResources(IDirect3DDevice9* d3d9Device)
	{		
		auto iterFind = mMapDeviceToTextureResources.find(d3d9Device);

		if (iterFind == mMapDeviceToTextureResources.end())		
			return nullptr;		
		
		return iterFind->second;
	}

	D3D9TextureCore::TextureResources* D3D9TextureCore::allocateTextureResources(IDirect3DDevice9* d3d9Device)
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
	
	void D3D9TextureCore::freeTextureResources(IDirect3DDevice9* d3d9Device, D3D9TextureCore::TextureResources* textureResources)
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
	
	UINT32 D3D9TextureCore::calculateSize() const
	{
		UINT32 instanceSize = mProperties.getNumFaces() * 
			PixelUtil::getMemorySize(mProperties.getWidth(), mProperties.getHeight(), 
			mProperties.getDepth(), mProperties.getFormat());

		return instanceSize * (UINT32)mMapDeviceToTextureResources.size();
	}
	
	void D3D9TextureCore::determinePool()
	{
		if (useDefaultPool())
			mD3DPool = D3DPOOL_DEFAULT;
		else
			mD3DPool = D3DPOOL_MANAGED;
	}
	
	void D3D9TextureCore::createInternalResources(IDirect3DDevice9* d3d9Device)
	{		
		switch (mProperties.getTextureType())
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
			BS_EXCEPT(InternalErrorException, "Unknown texture type.");
		}
	}

	void D3D9TextureCore::createNormTex(IDirect3DDevice9* d3d9Device)
	{
		UINT32 width = mProperties.getWidth();
		UINT32 height = mProperties.getHeight();
		int texUsage = mProperties.getUsage();
		UINT32 origNumMips = mProperties.getNumMipmaps();
		PixelFormat format = mProperties.getFormat();
		bool hwGamma = mProperties.isHardwareGammaEnabled();
		UINT32 sampleCount = mProperties.getMultisampleCount();

		assert(width > 0 || height > 0);

		D3DFORMAT d3dPF = chooseD3DFormat(d3d9Device);
		if (format != D3D9Mappings::_getPF(d3dPF))
		{
			BS_EXCEPT(RenderingAPIException, "Provided pixel format is not supported by the driver: " + toString(format));
		}

		// Use D3DX to help us create the texture, this way it can adjust any relevant sizes
		UINT numMips = (origNumMips == MIP_UNLIMITED) ? D3DX_DEFAULT : origNumMips + 1;

		DWORD usage = 0;
		if ((texUsage & TU_RENDERTARGET) != 0)
			usage = D3DUSAGE_RENDERTARGET;
		else if ((texUsage & TU_DEPTHSTENCIL) != 0)
			usage = D3DUSAGE_DEPTHSTENCIL;

		// Check dynamic textures
		if (texUsage & TU_DYNAMIC)
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
		if (hwGamma)
		{
			mHwGammaReadSupported = canUseHardwareGammaCorrection(d3d9Device, usage, D3DRTYPE_TEXTURE, d3dPF, false);
			if (texUsage & TU_RENDERTARGET)
				mHwGammaWriteSupported = canUseHardwareGammaCorrection(d3d9Device, usage, D3DRTYPE_TEXTURE, d3dPF, true);
		}

		// Check multisample level
		if ((texUsage & TU_RENDERTARGET) != 0 || (texUsage & TU_DEPTHSTENCIL) != 0)
		{
			D3D9RenderAPI* rsys = static_cast<D3D9RenderAPI*>(BansheeEngine::RenderAPICore::instancePtr());
			rsys->determineMultisampleSettings(d3d9Device, sampleCount, d3dPF, false, &mMultisampleType, &mMultisampleQuality);
		}
		else
		{
			mMultisampleType = D3DMULTISAMPLE_NONE;
			mMultisampleQuality = 0;
		}

		D3D9Device* device = D3D9RenderAPI::getDeviceManager()->getDeviceFromD3D9Device(d3d9Device);
		const D3DCAPS9& rkCurCaps = device->getD3D9DeviceCaps();			

		// Check if mip maps are supported on hardware
		if (numMips > 1 && (!(rkCurCaps.TextureCaps & D3DPTEXTURECAPS_MIPMAP) || (texUsage & TU_RENDERTARGET) != 0 || (texUsage & TU_DEPTHSTENCIL) != 0))
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

		if ((texUsage & TU_RENDERTARGET) != 0 && (mMultisampleType != D3DMULTISAMPLE_NONE))
		{
			// Create AA surface
			HRESULT hr = d3d9Device->CreateRenderTarget(width, height, d3dPF, 
				mMultisampleType, mMultisampleQuality,
				FALSE,
				&textureResources->pMultisampleSurface, NULL);

			if (FAILED(hr))
				BS_EXCEPT(RenderingAPIException, "Unable to create AA render target: " + String(DXGetErrorDescription(hr)));

			D3DSURFACE_DESC desc;
			hr = textureResources->pMultisampleSurface->GetDesc(&desc);
			if (FAILED(hr))
			{
				BS_EXCEPT(RenderingAPIException, "Can't get texture description: " + String(DXGetErrorDescription(hr)));
			}

			setFinalAttributes(d3d9Device, textureResources, desc.Width, desc.Height, 1, D3D9Mappings::_getPF(desc.Format));

			mIsBindableAsShaderResource = false; // Cannot bind AA surfaces
		}
		else if ((texUsage & TU_DEPTHSTENCIL) != 0 && (mMultisampleType != D3DMULTISAMPLE_NONE))
		{
			// Create AA depth stencil surface
			HRESULT hr = d3d9Device->CreateDepthStencilSurface(width, height, d3dPF, 
				mMultisampleType, mMultisampleQuality,
				FALSE, 
				&textureResources->pDepthStencilSurface, NULL);

			if (FAILED(hr))
				BS_EXCEPT(RenderingAPIException, "Unable to create AA depth stencil render target: " + String(DXGetErrorDescription(hr)));

			// Update final parameters as they may differ from requested ones
			D3DSURFACE_DESC desc;
			hr = textureResources->pDepthStencilSurface->GetDesc(&desc);
			if (FAILED(hr))
			{
				BS_EXCEPT(RenderingAPIException, "Can't get texture description: " + String(DXGetErrorDescription(hr)));
			}

			setFinalAttributes(d3d9Device, textureResources, desc.Width, desc.Height, 1, D3D9Mappings::_getPF(desc.Format));

			mIsBindableAsShaderResource = false; // Cannot bind AA depth buffer
		}
		else
		{
			// Create normal texture
			HRESULT hr = D3DXCreateTexture(d3d9Device, (UINT)width, (UINT)height, numMips,
				usage, d3dPF, mD3DPool, &textureResources->pNormTex);

			if (FAILED(hr))
			{
				BS_EXCEPT(RenderingAPIException, "Error creating texture: " + String(DXGetErrorDescription(hr)));
			}

			hr = textureResources->pNormTex->QueryInterface(IID_IDirect3DBaseTexture9, (void **)&textureResources->pBaseTex);
			if (FAILED(hr))
			{
				BS_EXCEPT(RenderingAPIException, "Can't get base texture: " + String(DXGetErrorDescription(hr)));
			}

			// Update final parameters as they may differ from requested ones
			D3DSURFACE_DESC desc;
			hr = textureResources->pNormTex->GetLevelDesc(0, &desc);
			if (FAILED(hr))
			{
				BS_EXCEPT(RenderingAPIException, "Can't get texture description: " + String(DXGetErrorDescription(hr)));
			}

			setFinalAttributes(d3d9Device, textureResources, desc.Width, desc.Height, 1, D3D9Mappings::_getPF(desc.Format));
		}
	}
	
	void D3D9TextureCore::createCubeTex(IDirect3DDevice9* d3d9Device)
	{
		UINT32 width = mProperties.getWidth();
		UINT32 height = mProperties.getHeight();
		int texUsage = mProperties.getUsage();
		UINT32 origNumMips = mProperties.getNumMipmaps();
		PixelFormat format = mProperties.getFormat();
		bool hwGamma = mProperties.isHardwareGammaEnabled();

		assert(width > 0 || height > 0);

		D3DFORMAT d3dPF = chooseD3DFormat(d3d9Device);
		if (format != D3D9Mappings::_getPF(d3dPF))
		{
			BS_EXCEPT(RenderingAPIException, "Provided pixel format is not supported by the driver: " + toString(format));
		}

		// Use D3DX to help us create the texture, this way it can adjust any relevant sizes
		DWORD usage = (texUsage & TU_RENDERTARGET) ? D3DUSAGE_RENDERTARGET : 0;
		usage |= (texUsage & TU_DEPTHSTENCIL) ? D3DUSAGE_DEPTHSTENCIL : 0;

		UINT numMips = (origNumMips == MIP_UNLIMITED) ? D3DX_DEFAULT : origNumMips + 1;

		// Check dynamic textures
		if (texUsage & TU_DYNAMIC)
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
		if (hwGamma)
		{
			mHwGammaReadSupported = canUseHardwareGammaCorrection(d3d9Device, usage, D3DRTYPE_CUBETEXTURE, d3dPF, false);
			if (texUsage & TU_RENDERTARGET)
				mHwGammaWriteSupported = canUseHardwareGammaCorrection(d3d9Device, usage, D3DRTYPE_CUBETEXTURE, d3dPF, true);
		}

		// No multisampling on cube textures
		mMultisampleType = D3DMULTISAMPLE_NONE;
		mMultisampleQuality = 0;

		D3D9Device* device = D3D9RenderAPI::getDeviceManager()->getDeviceFromD3D9Device(d3d9Device);
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
		HRESULT hr = D3DXCreateCubeTexture(d3d9Device, (UINT)width, numMips,
				usage, d3dPF, mD3DPool, &textureResources->pCubeTex);

		if (FAILED(hr))
		{
			BS_EXCEPT(RenderingAPIException, "Error creating texture: " + String(DXGetErrorDescription(hr)));
		}

		hr = textureResources->pCubeTex->QueryInterface(IID_IDirect3DBaseTexture9, (void **)&textureResources->pBaseTex);
		if (FAILED(hr))
		{
			BS_EXCEPT(RenderingAPIException, "Can't get base texture: " + String(DXGetErrorDescription(hr)));
		}
		
		// Update final parameters as they may differ from requested ones
		D3DSURFACE_DESC desc;
		hr = textureResources->pCubeTex->GetLevelDesc(0, &desc);
		if (FAILED(hr))
		{
			BS_EXCEPT(RenderingAPIException, "Can't get texture description: " + String(DXGetErrorDescription(hr)));
		}

		setFinalAttributes(d3d9Device, textureResources, desc.Width, desc.Height, 1, D3D9Mappings::_getPF(desc.Format));		
	}
	
	void D3D9TextureCore::createVolumeTex(IDirect3DDevice9* d3d9Device)
	{
		UINT32 width = mProperties.getWidth();
		UINT32 height = mProperties.getHeight();
		UINT32 depth = mProperties.getDepth();
		int texUsage = mProperties.getUsage();
		UINT32 origNumMips = mProperties.getNumMipmaps();
		PixelFormat format = mProperties.getFormat();
		bool hwGamma = mProperties.isHardwareGammaEnabled();

		assert(width > 0 && height > 0 && depth > 0);

		if (texUsage & TU_RENDERTARGET)
			BS_EXCEPT(RenderingAPIException, "D3D9 Volume texture can not be created as render target !!");

		if (texUsage & TU_DEPTHSTENCIL)
			BS_EXCEPT(RenderingAPIException, "D3D9 Volume texture can not be created as a depth stencil target !!");

		D3DFORMAT d3dPF = chooseD3DFormat(d3d9Device);
		if(format != D3D9Mappings::_getPF(d3dPF))
		{
			BS_EXCEPT(RenderingAPIException, "Provided pixel format is not supported by the driver: " + toString(format));
		}

		// Use D3DX to help us create the texture, this way it can adjust any relevant sizes
		DWORD usage = (texUsage & TU_RENDERTARGET) ? D3DUSAGE_RENDERTARGET : 0;
		UINT numMips = (origNumMips == MIP_UNLIMITED) ? D3DX_DEFAULT : origNumMips + 1;

		// Check dynamic textures
		if (texUsage & TU_DYNAMIC)
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
		if (hwGamma)
		{
			mHwGammaReadSupported = canUseHardwareGammaCorrection(d3d9Device, usage, D3DRTYPE_VOLUMETEXTURE, d3dPF, false);
			if (texUsage & TU_RENDERTARGET)
				mHwGammaWriteSupported = canUseHardwareGammaCorrection(d3d9Device, usage, D3DRTYPE_VOLUMETEXTURE, d3dPF, true);
		}

		D3D9Device* device = D3D9RenderAPI::getDeviceManager()->getDeviceFromD3D9Device(d3d9Device);
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
		HRESULT hr = D3DXCreateVolumeTexture(d3d9Device, (UINT)width, (UINT)height, (UINT)depth,
				numMips, usage, d3dPF, mD3DPool, &textureResources->pVolumeTex);

		if (FAILED(hr))
		{
			BS_EXCEPT(RenderingAPIException, "Error creating texture: " + String(DXGetErrorDescription(hr)));
		}

		hr = textureResources->pVolumeTex->QueryInterface(IID_IDirect3DBaseTexture9, (void**)&textureResources->pBaseTex);
		if (FAILED(hr))
		{
			BS_EXCEPT(RenderingAPIException, "Can't get base texture: " + String(DXGetErrorDescription(hr)));
		}
		
		// Update final parameters as they may differ from requested ones
		D3DVOLUME_DESC desc;
		hr = textureResources->pVolumeTex->GetLevelDesc(0, &desc);
		if (FAILED(hr))
		{
			BS_EXCEPT(RenderingAPIException, "Can't get texture description: " + String(DXGetErrorDescription(hr)));
		}

		setFinalAttributes(d3d9Device, textureResources, desc.Width, desc.Height, desc.Depth, D3D9Mappings::_getPF(desc.Format));
	}

	void D3D9TextureCore::setFinalAttributes(IDirect3DDevice9* d3d9Device, TextureResources* textureResources,
		UINT32 width, UINT32 height, UINT32 depth, PixelFormat format)
	{ 
		if(width != mProperties.getWidth() || height != mProperties.getHeight() || depth != mProperties.getDepth())
		{
			BS_EXCEPT(InternalErrorException, "Wanted and created textures sizes don't match!" \
				"Width: " + toString(width) + "/" + toString(mProperties.getWidth()) +
				"Height: " + toString(height) + "/" + toString(mProperties.getHeight()) +
				"Depth: " + toString(depth) + "/" + toString(mProperties.getDepth()));
		}

		if(format != mProperties.getFormat())
		{
			BS_EXCEPT(InternalErrorException, "Wanted and created texture formats don't match! " + 
				toString(format) + "/" + toString(mProperties.getFormat()));
		}
		
		createSurfaceList(d3d9Device, textureResources);
	}
	
	D3DTEXTUREFILTERTYPE D3D9TextureCore::getBestFilterMethod(IDirect3DDevice9* d3d9Device)
	{
		D3D9Device* device = D3D9RenderAPI::getDeviceManager()->getDeviceFromD3D9Device(d3d9Device);
		const D3DCAPS9& deviceCaps = device->getD3D9DeviceCaps();			
		
		DWORD filterCaps = 0;

		switch (mProperties.getTextureType())
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
	
	bool D3D9TextureCore::canUseDynamicTextures(IDirect3DDevice9* d3d9Device, DWORD srcUsage, D3DRESOURCETYPE srcType, 
		D3DFORMAT srcFormat)
	{		
		IDirect3D9* pD3D = nullptr;

		HRESULT hr = d3d9Device->GetDirect3D(&pD3D);
		if (FAILED(hr))
			BS_EXCEPT(InvalidParametersException, "GetDirect3D failed !!!");

		if (pD3D != nullptr)
			pD3D->Release();
	
		D3D9Device* device = D3D9RenderAPI::getDeviceManager()->getDeviceFromD3D9Device(d3d9Device);
		const D3DCAPS9& deviceCaps = device->getD3D9DeviceCaps();						
		D3DFORMAT backBufferFormat = device->getBackBufferFormat();

		hr = pD3D->CheckDeviceFormat(deviceCaps.AdapterOrdinal, deviceCaps.DeviceType, 
			backBufferFormat, srcUsage | D3DUSAGE_DYNAMIC, srcType, srcFormat);

		return hr == D3D_OK;
	}
	
	bool D3D9TextureCore::canUseHardwareGammaCorrection(IDirect3DDevice9* d3d9Device, DWORD srcUsage, 
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

		D3D9Device* device = D3D9RenderAPI::getDeviceManager()->getDeviceFromD3D9Device(d3d9Device);
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
	
	bool D3D9TextureCore::canAutoGenMipmaps(IDirect3DDevice9* d3d9Device, DWORD srcUsage, D3DRESOURCETYPE srcType, D3DFORMAT srcFormat)
	{
		IDirect3D9* pD3D = nullptr;

		HRESULT hr = d3d9Device->GetDirect3D(&pD3D);
		if (FAILED(hr))
		{
			BS_EXCEPT(InvalidParametersException, "GetDirect3D failed.");
		}

		if (pD3D != nullptr)
			pD3D->Release();

		D3D9Device* device = D3D9RenderAPI::getDeviceManager()->getDeviceFromD3D9Device(d3d9Device);
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
	
	D3DFORMAT D3D9TextureCore::chooseD3DFormat(IDirect3DDevice9* d3d9Device)
	{		
		// Choose frame buffer pixel format in case PF_UNKNOWN was requested
		if(mProperties.getFormat() == PF_UNKNOWN)
		{	
			D3D9Device* device = D3D9RenderAPI::getDeviceManager()->getDeviceFromD3D9Device(d3d9Device);
			
			if((mProperties.getUsage() & TU_DEPTHSTENCIL) != 0)
				return device->getDepthStencilFormat();
			else
				return device->getBackBufferFormat();		
		}

		// Choose closest supported D3D format as a D3D format
		return D3D9Mappings::_getPF(D3D9Mappings::_getClosestSupportedPF(mProperties.getFormat()));
	}
	
	void D3D9TextureCore::createSurfaceList(IDirect3DDevice9* d3d9Device, TextureResources* textureResources)
	{
		int texUsage = mProperties.getUsage();
		TextureType texType = mProperties.getTextureType();
		UINT32 numMips = mProperties.getNumMipmaps();

		assert(textureResources != nullptr);

		// Need to know static / dynamic
		UINT32 usage;
		if ((texUsage & TU_DYNAMIC) && mDynamicTextures)
		{
			usage = GBU_DYNAMIC;
		}
		else
		{
			usage = GBU_STATIC;
		}

		if (texUsage & TU_RENDERTARGET)
		{
			usage |= TU_RENDERTARGET;
		}
		else if (texUsage & TU_DEPTHSTENCIL)
		{
			usage |= TU_RENDERTARGET;
		}
		
		UINT32 surfaceCount = static_cast<UINT32>((mProperties.getNumFaces() * (numMips + 1)));
		bool updateOldList = mSurfaceList.size() == surfaceCount;
		if(!updateOldList)
		{			
			mSurfaceList.clear();
			for (UINT32 face = 0; face < mProperties.getNumFaces(); face++)
			{
				for (UINT32 mip = 0; mip <= numMips; mip++)
				{
					mSurfaceList.push_back(bs_shared_ptr_new<D3D9PixelBuffer>((GpuBufferUsage)usage, this));
				}
			}
		}

		IDirect3DSurface9* surface = nullptr;
		IDirect3DVolume9* volume = nullptr;

		if ((texUsage & TU_RENDERTARGET) != 0 && (mMultisampleType != D3DMULTISAMPLE_NONE))
		{
			assert(textureResources->pMultisampleSurface);
			assert(texType == TEX_TYPE_2D);

			D3D9PixelBuffer* currPixelBuffer = static_cast<D3D9PixelBuffer*>(mSurfaceList[0].get());

			currPixelBuffer->bind(d3d9Device, textureResources->pMultisampleSurface, textureResources->pBaseTex);
		}
		else if ((texUsage & TU_DEPTHSTENCIL) != 0 && (mMultisampleType != D3DMULTISAMPLE_NONE))
		{
			assert(textureResources->pDepthStencilSurface);
			assert(texType == TEX_TYPE_2D);

			D3D9PixelBuffer* currPixelBuffer = static_cast<D3D9PixelBuffer*>(mSurfaceList[0].get());

			currPixelBuffer->bind(d3d9Device, textureResources->pDepthStencilSurface, textureResources->pBaseTex);
		}
		else
		{
			assert(textureResources->pBaseTex);

			UINT32 numCreatedMips = textureResources->pBaseTex->GetLevelCount() - 1;
			if (numCreatedMips != numMips)
			{
				BS_EXCEPT(InternalErrorException, "Number of created and wanted mip map levels doesn't match: " + 
					toString(numCreatedMips) + "/" + toString(numMips));
			}

			switch(texType) 
			{
			case TEX_TYPE_2D:
			case TEX_TYPE_1D:
				assert(textureResources->pNormTex);

				for (UINT32 mip = 0; mip <= numMips; mip++)
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
					for (UINT32 mip = 0; mip <= numMips; mip++)
					{
						if(textureResources->pCubeTex->GetCubeMapSurface((D3DCUBEMAP_FACES)face, static_cast<UINT>(mip), &surface) != D3D_OK)
							BS_EXCEPT(RenderingAPIException, "Get cubemap surface failed");

						UINT32 idx = face*(numMips + 1) + mip;
						D3D9PixelBuffer* currPixelBuffer = static_cast<D3D9PixelBuffer*>(mSurfaceList[idx].get());

						currPixelBuffer->bind(d3d9Device, surface, textureResources->pBaseTex);

						surface->Release();				
					}				
				}
				break;
			case TEX_TYPE_3D:
				assert(textureResources->pVolumeTex);

				for (UINT32 mip = 0; mip <= numMips; mip++)
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

	PixelBufferPtr D3D9TextureCore::getBuffer(UINT32 face, UINT32 mipmap) 
	{
		THROW_IF_NOT_CORE_THREAD;

		if(face >= mProperties.getNumFaces())
			BS_EXCEPT(InvalidParametersException, "A three dimensional cube has six faces");
		if (mipmap > mProperties.getNumMipmaps())
			BS_EXCEPT(InvalidParametersException, "Mipmap index out of range");

		UINT32 idx = face*(mProperties.getNumMipmaps() + 1) + mipmap;

		IDirect3DDevice9* d3d9Device = D3D9RenderAPI::getActiveD3D9Device();
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

	bool D3D9TextureCore::useDefaultPool()
	{
		int usage = mProperties.getUsage();
		return (usage & TU_RENDERTARGET) || (usage & TU_DEPTHSTENCIL) || ((usage & TU_DYNAMIC) && mDynamicTextures);
	}
	
	void D3D9TextureCore::notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device) 
	{		
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		if (D3D9RenderAPI::getResourceManager()->getCreationPolicy() == RCP_CREATE_ON_ALL_DEVICES)
			createInternalResources(d3d9Device);
	}

	void D3D9TextureCore::notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device) 
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

	void D3D9TextureCore::notifyOnDeviceLost(IDirect3DDevice9* d3d9Device) 
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

	void D3D9TextureCore::notifyOnDeviceReset(IDirect3DDevice9* d3d9Device) 
	{		
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		if(mD3DPool == D3DPOOL_DEFAULT)
		{			
			createInternalResources(d3d9Device);
		}
	}

	IDirect3DBaseTexture9* D3D9TextureCore::getTexture_internal()
	{
		THROW_IF_NOT_CORE_THREAD;

		TextureResources* textureResources;			
		IDirect3DDevice9* d3d9Device = D3D9RenderAPI::getActiveD3D9Device();
			
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

	IDirect3DTexture9* D3D9TextureCore::getNormTexture_internal()
	{
		THROW_IF_NOT_CORE_THREAD;

		TextureResources* textureResources;
		IDirect3DDevice9* d3d9Device = D3D9RenderAPI::getActiveD3D9Device();
		
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

	IDirect3DCubeTexture9* D3D9TextureCore::getCubeTexture_internal()
	{
		THROW_IF_NOT_CORE_THREAD;

		TextureResources* textureResources;
		IDirect3DDevice9* d3d9Device = D3D9RenderAPI::getActiveD3D9Device();
		
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
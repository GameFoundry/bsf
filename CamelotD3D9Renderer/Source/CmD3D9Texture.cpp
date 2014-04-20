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
#include "CmCoreThread.h"
#include "CmD3D9Texture.h"
#include "CmD3D9PixelBuffer.h"
#include "CmException.h"
#include "CmBitwise.h"
#include "CmD3D9Mappings.h"
#include "CmD3D9RenderSystem.h"
#include "CmD3D9TextureManager.h"
#include "CmD3D9Device.h"
#include "CmD3D9DeviceManager.h"
#include "CmD3D9ResourceManager.h"

namespace BansheeEngine 
{
	
    D3D9Texture::D3D9Texture()
        :Texture(),              
        mD3DPool(D3DPOOL_MANAGED),
		mDynamicTextures(false),
		mHwGammaReadSupported(false),
		mHwGammaWriteSupported(false),	
		mFSAAType(D3DMULTISAMPLE_NONE),
		mFSAAQuality(0),
		mIsBindableAsShaderResource(true)
	{
       
	}
	
	D3D9Texture::~D3D9Texture()
	{	
	}

	PixelData D3D9Texture::lockImpl(GpuLockOptions options, UINT32 mipLevel, UINT32 face)
	{
		if(mLockedBuffer != nullptr)
			CM_EXCEPT(InternalErrorException, "Trying to lock a buffer that's already locked.");

		if(getUsage() == TU_DEPTHSTENCIL)
			CM_EXCEPT(InternalErrorException, "Cannot lock a depth stencil texture.");

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
			CM_EXCEPT(InternalErrorException, "Trying to unlock a buffer that's not locked.");

		mLockedBuffer->unlock();
		mLockedBuffer = nullptr;
	}

	void D3D9Texture::readData(PixelData& dest, UINT32 mipLevel, UINT32 face)
	{
		PixelData myData = lock(GBL_READ_ONLY, mipLevel, face);

#if CM_DEBUG_MODE
		if(dest.getConsecutiveSize() != myData.getConsecutiveSize())
		{
			unlock();
			CM_EXCEPT(InternalErrorException, "Buffer sizes don't match");
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
			CM_EXCEPT(RenderingAPIException, "Trying to write into a buffer with unsupported usage: " + toString(mUsage));
		}
	}
	
	void D3D9Texture::copyImpl(TexturePtr& target)
	{
		THROW_IF_NOT_CORE_THREAD;

        // check if this & target are the same format and type
		// blitting from or to cube textures is not supported yet
		if (target->getUsage() != getUsage() ||
			target->getTextureType() != getTextureType())
		{
			CM_EXCEPT(InvalidParametersException, 
					"Src. and dest. textures must be of same type and must have the same usage !!!");
		}

        HRESULT hr;
        D3D9Texture *other;
		// get the target
		other = reinterpret_cast< D3D9Texture * >( target.get() );
		// target rectangle (whole surface)
		RECT dstRC = {0, 0, static_cast<LONG>(other->getWidth()), static_cast<LONG>(other->getHeight())};

		DeviceToTextureResourcesIterator it = mMapDeviceToTextureResources.begin();
		while (it != mMapDeviceToTextureResources.end())
		{
			TextureResources* srcTextureResource = it->second;
			TextureResources* dstTextureResource = other->getTextureResources(it->first);

	
			// do it plain for normal texture
			if (getTextureType() == TEX_TYPE_2D &&
				srcTextureResource->pNormTex && 
				dstTextureResource->pNormTex)
			{			
				// get our source surface
				IDirect3DSurface9 *pSrcSurface = 0;
				if( FAILED( hr = srcTextureResource->pNormTex->GetSurfaceLevel(0, &pSrcSurface) ) )
				{
					String msg = DXGetErrorDescription(hr);
					CM_EXCEPT(RenderingAPIException, "Couldn't blit : " + msg);
				}

				// get our target surface
				IDirect3DSurface9 *pDstSurface = 0;			
				if( FAILED( hr = dstTextureResource->pNormTex->GetSurfaceLevel(0, &pDstSurface) ) )
				{
					String msg = DXGetErrorDescription(hr);
					SAFE_RELEASE(pSrcSurface);
					CM_EXCEPT(RenderingAPIException, "Couldn't blit : " + msg );
				}

				// do the blit, it's called StretchRect in D3D9 :)
				if( FAILED( hr = it->first->StretchRect( pSrcSurface, NULL, pDstSurface, &dstRC, D3DTEXF_NONE) ) )
				{
					String msg = DXGetErrorDescription(hr);
					SAFE_RELEASE(pSrcSurface);
					SAFE_RELEASE(pDstSurface);
					CM_EXCEPT(RenderingAPIException, "Couldn't blit : " + msg);
				}

				// release temp. surfaces
				SAFE_RELEASE(pSrcSurface);
				SAFE_RELEASE(pDstSurface);
			}
			else if (getTextureType() == TEX_TYPE_CUBE_MAP &&
				srcTextureResource->pCubeTex && 
				dstTextureResource->pCubeTex)
			{				
				// blit to 6 cube faces
				for (UINT32 face = 0; face < 6; face++)
				{
					// get our source surface
					IDirect3DSurface9 *pSrcSurface = 0;
					if( FAILED( hr =srcTextureResource->pCubeTex->GetCubeMapSurface((D3DCUBEMAP_FACES)face, 0, &pSrcSurface) ) )
					{
						String msg = DXGetErrorDescription(hr);
						CM_EXCEPT(RenderingAPIException, "Couldn't blit : " + msg);
					}

					// get our target surface
					IDirect3DSurface9 *pDstSurface = 0;
					if( FAILED( hr = dstTextureResource->pCubeTex->GetCubeMapSurface((D3DCUBEMAP_FACES)face, 0, &pDstSurface) ) )
					{
						String msg = DXGetErrorDescription(hr);
						SAFE_RELEASE(pSrcSurface);
						CM_EXCEPT(RenderingAPIException, "Couldn't blit : " + msg);
					}

					// do the blit, it's called StretchRect in D3D9 :)
					if( FAILED( hr = it->first->StretchRect( pSrcSurface, NULL, pDstSurface, &dstRC, D3DTEXF_NONE) ) )
					{
						String msg = DXGetErrorDescription(hr);
						SAFE_RELEASE(pSrcSurface);
						SAFE_RELEASE(pDstSurface);
						CM_EXCEPT(RenderingAPIException, "Couldn't blit : " + msg);
					}

					// release temp. surfaces
					SAFE_RELEASE(pSrcSurface);
					SAFE_RELEASE(pDstSurface);
				}
			}
			else
			{
				CM_EXCEPT(NotImplementedException, "Copy to texture is implemented only for 2D and cube textures !!!");
			}

			++it;
		}		
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

		DeviceToTextureResourcesIterator it = mMapDeviceToTextureResources.begin();
		while (it != mMapDeviceToTextureResources.end())
		{
			TextureResources* textureResource = it->second;

			freeTextureResources(it->first, textureResource);			
			++it;
		}

		it = mMapDeviceToTextureResources.begin();
		while (it != mMapDeviceToTextureResources.end())
		{
			TextureResources* textureResource = it->second;

			if(textureResource != nullptr)
				cm_delete<PoolAlloc>(textureResource);

			++it;
		}		

		mMapDeviceToTextureResources.clear();
		mSurfaceList.clear();	
		
		clearBufferViews();

		Texture::destroy_internal();
	}
	
	D3D9Texture::TextureResources* D3D9Texture::getTextureResources(IDirect3DDevice9* d3d9Device)
	{		
		DeviceToTextureResourcesIterator it = mMapDeviceToTextureResources.find(d3d9Device);

		if (it == mMapDeviceToTextureResources.end())		
			return NULL;		
		
		return it->second;
	}

	
	D3D9Texture::TextureResources* D3D9Texture::allocateTextureResources(IDirect3DDevice9* d3d9Device)
	{
		assert(mMapDeviceToTextureResources.find(d3d9Device) == mMapDeviceToTextureResources.end());

		TextureResources* textureResources = cm_new<TextureResources, PoolAlloc>();

		textureResources->pNormTex		= NULL;
		textureResources->pCubeTex		= NULL;
		textureResources->pVolumeTex	= NULL;
		textureResources->pBaseTex		= NULL;
		textureResources->pFSAASurface	= NULL;
		textureResources->pDepthStencilSurface = NULL;

		mMapDeviceToTextureResources[d3d9Device] = textureResources;

		return textureResources;
	}

	
	void D3D9Texture::createTextureResources(IDirect3DDevice9* d3d9Device)
	{				
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION
			
			/*		prepareImpl();		

			loadImpl(d3d9Device);

			postLoadImpl();	*/			
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
		SAFE_RELEASE(textureResources->pFSAASurface);
		SAFE_RELEASE(textureResources->pDepthStencilSurface);
	}
	
	UINT32 D3D9Texture::calculateSize(void) const
	{
		UINT32 instanceSize = getNumFaces() * PixelUtil::getMemorySize(mWidth, mHeight, mDepth, mFormat);

		return instanceSize * (UINT32)mMapDeviceToTextureResources.size();
	}
	
	void D3D9Texture::determinePool()
	{
		if (useDefaultPool())
		{
			mD3DPool = D3DPOOL_DEFAULT;
		}
		else
		{
			mD3DPool = D3DPOOL_MANAGED;
		}

	}
	
	void D3D9Texture::createInternalResources(IDirect3DDevice9* d3d9Device)
	{		
		TextureResources* textureResources;			

		// Check if resources already exist.
		textureResources = getTextureResources(d3d9Device);
		if (textureResources != NULL && textureResources->pBaseTex != NULL)
			return;
		
		// load based on tex.type
		switch (getTextureType())
		{
		case TEX_TYPE_1D:
		case TEX_TYPE_2D:
			_createNormTex(d3d9Device);
			break;
		case TEX_TYPE_CUBE_MAP:
			_createCubeTex(d3d9Device);
			break;
		case TEX_TYPE_3D:
			_createVolumeTex(d3d9Device);
			break;
		default:
			destroy_internal();
			CM_EXCEPT(InternalErrorException, "Unknown texture type");
		}
	}

	
	void D3D9Texture::_createNormTex(IDirect3DDevice9* d3d9Device)
	{
		// we must have those defined here
		assert(mWidth > 0 || mHeight > 0);

		// determine wich D3D9 pixel format we'll use
		HRESULT hr;
		D3DFORMAT d3dPF = _chooseD3DFormat(d3d9Device);
		
		if(mFormat != D3D9Mappings::_getPF(d3dPF))
		{
			CM_EXCEPT(RenderingAPIException, "Provided pixel format is not supported by the driver: " + toString(mFormat));
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
			if (_canUseDynamicTextures(d3d9Device, usage, D3DRTYPE_TEXTURE, d3dPF))
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
			mHwGammaReadSupported = _canUseHardwareGammaCorrection(d3d9Device, usage, D3DRTYPE_TEXTURE, d3dPF, false);
			if (mUsage & TU_RENDERTARGET)
				mHwGammaWriteSupported = _canUseHardwareGammaCorrection(d3d9Device, usage, D3DRTYPE_TEXTURE, d3dPF, true);
		}

		// Check FSAA level
		if ((mUsage & TU_RENDERTARGET) != 0 || (mUsage & TU_DEPTHSTENCIL) != 0)
		{
			D3D9RenderSystem* rsys = static_cast<D3D9RenderSystem*>(BansheeEngine::RenderSystem::instancePtr());
			rsys->determineFSAASettings(d3d9Device, mFSAA, mFSAAHint, d3dPF, false, 
				&mFSAAType, &mFSAAQuality);
		}
		else
		{
			mFSAAType = D3DMULTISAMPLE_NONE;
			mFSAAQuality = 0;
		}

		D3D9Device* device = D3D9RenderSystem::getDeviceManager()->getDeviceFromD3D9Device(d3d9Device);
		const D3DCAPS9& rkCurCaps = device->getD3D9DeviceCaps();			

		// check if mip maps are supported on hardware
		if (numMips > 1 && (!(rkCurCaps.TextureCaps & D3DPTEXTURECAPS_MIPMAP) || (mUsage & TU_RENDERTARGET) != 0 || (mUsage & TU_DEPTHSTENCIL) != 0))
		{
			CM_EXCEPT(InvalidParametersException, "Invalid number of mipmaps. Maximum allowed is: 0");
			numMips = 1;
		}

		// derive the pool to use
		determinePool();

		TextureResources* textureResources;			
	
		// Get or create new texture resources structure.
		textureResources = getTextureResources(d3d9Device);
		if (textureResources != NULL)
			freeTextureResources(d3d9Device, textureResources);
		else
			textureResources = allocateTextureResources(d3d9Device);

		if ((mUsage & TU_RENDERTARGET) != 0 && (mFSAAType != D3DMULTISAMPLE_NONE))
		{
			// Create AA surface
			HRESULT hr = d3d9Device->CreateRenderTarget(mWidth, mHeight, d3dPF, 
				mFSAAType, 
				mFSAAQuality,
				TRUE, // TODO - Possible performance issues? Need to check
				&textureResources->pFSAASurface, NULL);

			if (FAILED(hr))
			{
				CM_EXCEPT(RenderingAPIException, "Unable to create AA render target: " + String(DXGetErrorDescription(hr)));
			}

			D3DSURFACE_DESC desc;
			hr = textureResources->pFSAASurface->GetDesc(&desc);
			if (FAILED(hr))
			{
				destroy_internal();
				CM_EXCEPT(RenderingAPIException, "Can't get texture description: " + String(DXGetErrorDescription(hr)));
			}

			_setFinalAttributes(d3d9Device, textureResources, 
				desc.Width, desc.Height, 1, D3D9Mappings::_getPF(desc.Format));

			mIsBindableAsShaderResource = true; // Cannot bind AA surfaces
		}
		else if ((mUsage & TU_DEPTHSTENCIL) != 0 && (mFSAAType != D3DMULTISAMPLE_NONE))
		{
			// Create AA depth stencil surface
			HRESULT hr = d3d9Device->CreateDepthStencilSurface(mWidth, mHeight, d3dPF, 
				mFSAAType, 
				mFSAAQuality,
				TRUE, // TODO - Possible performance issues? Need to check
				&textureResources->pDepthStencilSurface, NULL);

			if (FAILED(hr))
				CM_EXCEPT(RenderingAPIException, "Unable to create AA depth stencil render target: " + String(DXGetErrorDescription(hr)));

			D3DSURFACE_DESC desc;
			hr = textureResources->pDepthStencilSurface->GetDesc(&desc);
			if (FAILED(hr))
			{
				destroy_internal();
				CM_EXCEPT(RenderingAPIException, "Can't get texture description: " + String(DXGetErrorDescription(hr)));
			}

			_setFinalAttributes(d3d9Device, textureResources, desc.Width, desc.Height, 1, D3D9Mappings::_getPF(desc.Format));

			mIsBindableAsShaderResource = true; // Cannot bind AA depth buffer
		}
		else
		{
			// create the texture
			hr = D3DXCreateTexture(	
				d3d9Device,								// device
				static_cast<UINT>(mWidth),				// width
				static_cast<UINT>(mHeight),				// height
				numMips,								// number of mip map levels
				usage,									// usage
				d3dPF,									// pixel format
				mD3DPool,
				&textureResources->pNormTex);			// data pointer

			// check result and except if failed
			if (FAILED(hr))
			{
				destroy_internal();
				CM_EXCEPT(RenderingAPIException, "Error creating texture: " + String(DXGetErrorDescription(hr)));
			}

			// set the base texture we'll use in the render system
			hr = textureResources->pNormTex->QueryInterface(IID_IDirect3DBaseTexture9, (void **)&textureResources->pBaseTex);
			if (FAILED(hr))
			{
				destroy_internal();
				CM_EXCEPT(RenderingAPIException, "Can't get base texture: " + String(DXGetErrorDescription(hr)));
			}

			// set final tex. attributes from tex. description
			// they may differ from the source image !!!
			D3DSURFACE_DESC desc;
			hr = textureResources->pNormTex->GetLevelDesc(0, &desc);
			if (FAILED(hr))
			{
				destroy_internal();
				CM_EXCEPT(RenderingAPIException, "Can't get texture description: " + String(DXGetErrorDescription(hr)));
			}

			_setFinalAttributes(d3d9Device, textureResources, 
				desc.Width, desc.Height, 1, D3D9Mappings::_getPF(desc.Format));
		}
	}
	
	void D3D9Texture::_createCubeTex(IDirect3DDevice9* d3d9Device)
	{
		// we must have those defined here
		assert(mWidth > 0 || mHeight > 0);

		if (mUsage & TU_RENDERTARGET)
			CM_EXCEPT(RenderingAPIException, "D3D9 Cube texture can not be created as render target !!");

		if (mUsage & TU_DEPTHSTENCIL)
			CM_EXCEPT(RenderingAPIException, "D3D9 Cube texture can not be created as a depth stencil target !!");

		// determine wich D3D9 pixel format we'll use
		HRESULT hr;
		D3DFORMAT d3dPF = _chooseD3DFormat(d3d9Device);

		if(mFormat != D3D9Mappings::_getPF(d3dPF))
		{
			CM_EXCEPT(RenderingAPIException, "Provided pixel format is not supported by the driver: " + toString(mFormat));
		}

		// Use D3DX to help us create the texture, this way it can adjust any relevant sizes
		DWORD usage = (mUsage & TU_RENDERTARGET) ? D3DUSAGE_RENDERTARGET : 0;
		UINT numMips = (mNumMipmaps == MIP_UNLIMITED) ? 
			D3DX_DEFAULT : mNumMipmaps + 1;
		// Check dynamic textures
		if (mUsage & TU_DYNAMIC)
		{
			if (_canUseDynamicTextures(d3d9Device, usage, D3DRTYPE_CUBETEXTURE, d3dPF))
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
			mHwGammaReadSupported = _canUseHardwareGammaCorrection(d3d9Device, usage, D3DRTYPE_CUBETEXTURE, d3dPF, false);
			if (mUsage & TU_RENDERTARGET)
				mHwGammaWriteSupported = _canUseHardwareGammaCorrection(d3d9Device, usage, D3DRTYPE_CUBETEXTURE, d3dPF, true);
		}

		mFSAAType = D3DMULTISAMPLE_NONE;
		mFSAAQuality = 0;

		D3D9Device* device = D3D9RenderSystem::getDeviceManager()->getDeviceFromD3D9Device(d3d9Device);
		const D3DCAPS9& rkCurCaps = device->getD3D9DeviceCaps();			
		
		// check if mip map cube textures are supported
		if (numMips > 1 && !(rkCurCaps.TextureCaps & D3DPTEXTURECAPS_MIPCUBEMAP))
		{
			CM_EXCEPT(InvalidParametersException, "Invalid number of mipmaps. Maximum allowed is: 0");
			numMips = 1;
		}

		// derive the pool to use
		determinePool();
		TextureResources* textureResources;			

		// Get or create new texture resources structure.
		textureResources = getTextureResources(d3d9Device);
		if (textureResources != NULL)
			freeTextureResources(d3d9Device, textureResources);
		else
			textureResources = allocateTextureResources(d3d9Device);

		// create the texture
		hr = D3DXCreateCubeTexture(	
				d3d9Device,									// device
				static_cast<UINT>(mWidth),		 			// dimension
				numMips,							 		// number of mip map levels
				usage,								 		// usage
				d3dPF,								 		// pixel format
				mD3DPool,
				&textureResources->pCubeTex);		 		// data pointer
		// check result and except if failed
		if (FAILED(hr))
		{
			destroy_internal();
			CM_EXCEPT(RenderingAPIException, "Error creating texture: " + String(DXGetErrorDescription(hr)));
		}

		// set the base texture we'll use in the render system
		hr = textureResources->pCubeTex->QueryInterface(IID_IDirect3DBaseTexture9, (void **)&textureResources->pBaseTex);
		if (FAILED(hr))
		{
			destroy_internal();
			CM_EXCEPT(RenderingAPIException, "Can't get base texture: " + String(DXGetErrorDescription(hr)));
		}
		
		// set final tex. attributes from tex. description
		// they may differ from the source image !!!
		D3DSURFACE_DESC desc;
		hr = textureResources->pCubeTex->GetLevelDesc(0, &desc);
		if (FAILED(hr))
		{
			destroy_internal();
			CM_EXCEPT(RenderingAPIException, "Can't get texture description: " + String(DXGetErrorDescription(hr)));
		}

		_setFinalAttributes(d3d9Device, textureResources, 
			desc.Width, desc.Height, 1, D3D9Mappings::_getPF(desc.Format));		
	}
	
	void D3D9Texture::_createVolumeTex(IDirect3DDevice9* d3d9Device)
	{
		// we must have those defined here
		assert(mWidth > 0 && mHeight > 0 && mDepth>0);

		if (mUsage & TU_RENDERTARGET)
			CM_EXCEPT(RenderingAPIException, "D3D9 Volume texture can not be created as render target !!");

		if (mUsage & TU_DEPTHSTENCIL)
			CM_EXCEPT(RenderingAPIException, "D3D9 Volume texture can not be created as a depth stencil target !!");

		// determine which D3D9 pixel format we'll use
		HRESULT hr;
		D3DFORMAT d3dPF = _chooseD3DFormat(d3d9Device);

		if(mFormat != D3D9Mappings::_getPF(d3dPF))
		{
			CM_EXCEPT(RenderingAPIException, "Provided pixel format is not supported by the driver: " + toString(mFormat));
		}

		// Use D3DX to help us create the texture, this way it can adjust any relevant sizes
		DWORD usage = (mUsage & TU_RENDERTARGET) ? D3DUSAGE_RENDERTARGET : 0;
		UINT numMips = (mNumMipmaps == MIP_UNLIMITED) ? 
			D3DX_DEFAULT : mNumMipmaps + 1;
		// Check dynamic textures
		if (mUsage & TU_DYNAMIC)
		{
			if (_canUseDynamicTextures(d3d9Device, usage, D3DRTYPE_VOLUMETEXTURE, d3dPF))
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
			mHwGammaReadSupported = _canUseHardwareGammaCorrection(d3d9Device, usage, D3DRTYPE_VOLUMETEXTURE, d3dPF, false);
			if (mUsage & TU_RENDERTARGET)
				mHwGammaWriteSupported = _canUseHardwareGammaCorrection(d3d9Device, usage, D3DRTYPE_VOLUMETEXTURE, d3dPF, true);
		}

		D3D9Device* device = D3D9RenderSystem::getDeviceManager()->getDeviceFromD3D9Device(d3d9Device);
		const D3DCAPS9& rkCurCaps = device->getD3D9DeviceCaps();			


		// check if mip map volume textures are supported
		if (numMips > 1 && !(rkCurCaps.TextureCaps & D3DPTEXTURECAPS_MIPVOLUMEMAP))
		{
			CM_EXCEPT(InvalidParametersException, "Invalid number of mipmaps. Maximum allowed is: 0");
		}

		// derive the pool to use
		determinePool();
		TextureResources* textureResources;			

		// Get or create new texture resources structure.
		textureResources = getTextureResources(d3d9Device);
		if (textureResources != NULL)
			freeTextureResources(d3d9Device, textureResources);
		else
			textureResources = allocateTextureResources(d3d9Device);

		// create the texture
		hr = D3DXCreateVolumeTexture(	
				d3d9Device,									// device
				static_cast<UINT>(mWidth),					// dimension
				static_cast<UINT>(mHeight),
				static_cast<UINT>(mDepth),
				numMips,									// number of mip map levels
				usage,										// usage
				d3dPF,										// pixel format
				mD3DPool,
				&textureResources->pVolumeTex);				// data pointer
		// check result and except if failed
		if (FAILED(hr))
		{
			destroy_internal();
			CM_EXCEPT(RenderingAPIException, "Error creating texture: " + String(DXGetErrorDescription(hr)));
		}

		// set the base texture we'll use in the render system
		hr = textureResources->pVolumeTex->QueryInterface(IID_IDirect3DBaseTexture9, (void **)&textureResources->pBaseTex);
		if (FAILED(hr))
		{
			destroy_internal();
			CM_EXCEPT(RenderingAPIException, "Can't get base texture: " + String(DXGetErrorDescription(hr)));
		}
		
		// set final tex. attributes from tex. description
		// they may differ from the source image !!!
		D3DVOLUME_DESC desc;
		hr = textureResources->pVolumeTex->GetLevelDesc(0, &desc);
		if (FAILED(hr))
		{
			destroy_internal();
			CM_EXCEPT(RenderingAPIException, "Can't get texture description: " + String(DXGetErrorDescription(hr)));
		}
		_setFinalAttributes(d3d9Device, textureResources,
			desc.Width, desc.Height, desc.Depth, D3D9Mappings::_getPF(desc.Format));
	}

	
	void D3D9Texture::_setFinalAttributes(IDirect3DDevice9* d3d9Device, 
		TextureResources* textureResources,
		UINT32 width, UINT32 height, 
        UINT32 depth, PixelFormat format)
	{ 
		if(width != mWidth || height != mHeight || depth != mDepth)
		{
			CM_EXCEPT(InternalErrorException, "Wanted and created textures sizes don't match!" \
				"Width: " + toString(width) + "/" + toString(mWidth) +
				"Height: " + toString(height) + "/" + toString(mHeight) +
				"Depth: " + toString(depth) + "/" + toString(mDepth));
		}

		if(format != mFormat)
		{
			CM_EXCEPT(InternalErrorException, "Wanted and created texture formats don't match! " + toString(format) + "/" + toString(mFormat));
		}
		
		// Create list of subsurfaces for getBuffer()
		_createSurfaceList(d3d9Device, textureResources);
	}
	
	D3DTEXTUREFILTERTYPE D3D9Texture::_getBestFilterMethod(IDirect3DDevice9* d3d9Device)
	{
		D3D9Device* device = D3D9RenderSystem::getDeviceManager()->getDeviceFromD3D9Device(d3d9Device);
		const D3DCAPS9& rkCurCaps = device->getD3D9DeviceCaps();			
		
		
		DWORD filterCaps = 0;
		// Minification filter is used for mipmap generation
		// Pick the best one supported for this tex type
		switch (getTextureType())
		{
		case TEX_TYPE_1D:		// Same as 2D
		case TEX_TYPE_2D:		filterCaps = rkCurCaps.TextureFilterCaps;	break;
		case TEX_TYPE_3D:		filterCaps = rkCurCaps.VolumeTextureFilterCaps;	break;
		case TEX_TYPE_CUBE_MAP:	filterCaps = rkCurCaps.CubeTextureFilterCaps;	break;
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
	
	bool D3D9Texture::_canUseDynamicTextures(IDirect3DDevice9* d3d9Device, 
		DWORD srcUsage, 
		D3DRESOURCETYPE srcType, 
		D3DFORMAT srcFormat)
	{		
		HRESULT hr;
		IDirect3D9* pD3D = NULL;

		hr = d3d9Device->GetDirect3D(&pD3D);
		if (FAILED(hr))
		{
			CM_EXCEPT(InvalidParametersException, "GetDirect3D failed !!!");
		}
		if (pD3D != NULL)
			pD3D->Release();
	
		D3D9Device* device = D3D9RenderSystem::getDeviceManager()->getDeviceFromD3D9Device(d3d9Device);
		const D3DCAPS9& rkCurCaps = device->getD3D9DeviceCaps();						
		D3DFORMAT eBackBufferFormat = device->getBackBufferFormat();

		
		// Check for dynamic texture support
		
		// check for auto gen. mip maps support
		hr = pD3D->CheckDeviceFormat(
			rkCurCaps.AdapterOrdinal, 
			rkCurCaps.DeviceType, 
			eBackBufferFormat, 
			srcUsage | D3DUSAGE_DYNAMIC,
			srcType,
			srcFormat);

		if (hr == D3D_OK)
			return true;
		else
			return false;
	}
	
	bool D3D9Texture::_canUseHardwareGammaCorrection(IDirect3DDevice9* d3d9Device,
		DWORD srcUsage, 
		D3DRESOURCETYPE srcType, D3DFORMAT srcFormat, bool forwriting)
	{
		HRESULT hr;
		IDirect3D9* pD3D = NULL;

		hr = d3d9Device->GetDirect3D(&pD3D);
		if (FAILED(hr))
		{
			CM_EXCEPT(InvalidParametersException, "GetDirect3D failed !!!" );
		}
		if (pD3D != NULL)
			pD3D->Release();

		D3D9Device* device = D3D9RenderSystem::getDeviceManager()->getDeviceFromD3D9Device(d3d9Device);
		const D3DCAPS9& rkCurCaps = device->getD3D9DeviceCaps();						
		D3DFORMAT eBackBufferFormat = device->getBackBufferFormat();


		// Always check 'read' capability here
		// We will check 'write' capability only in the context of a render target
		if (forwriting)
			srcUsage |= D3DUSAGE_QUERY_SRGBWRITE;
		else
			srcUsage |= D3DUSAGE_QUERY_SRGBREAD;

		// Check for sRGB support		
		// check for auto gen. mip maps support
		hr = pD3D->CheckDeviceFormat(
			rkCurCaps.AdapterOrdinal, 
			rkCurCaps.DeviceType, 
			eBackBufferFormat, 
			srcUsage,
			srcType,
			srcFormat);
		if (hr == D3D_OK)
			return true;
		else
			return false;

	}
	
	bool D3D9Texture::_canAutoGenMipmaps(IDirect3DDevice9* d3d9Device, 
		DWORD srcUsage, D3DRESOURCETYPE srcType, D3DFORMAT srcFormat)
	{
		HRESULT hr;
		IDirect3D9* pD3D = NULL;

		hr = d3d9Device->GetDirect3D(&pD3D);
		if (FAILED(hr))
		{
			CM_EXCEPT(InvalidParametersException, "GetDirect3D failed !!!");
		}
		if (pD3D != NULL)
			pD3D->Release();

		D3D9Device* device = D3D9RenderSystem::getDeviceManager()->getDeviceFromD3D9Device(d3d9Device);
		const D3DCAPS9& rkCurCaps = device->getD3D9DeviceCaps();						
		D3DFORMAT eBackBufferFormat = device->getBackBufferFormat();


		// Hacky override - many (all?) cards seem to not be able to autogen on 
		// textures which are not a power of two
		// Can we even mipmap on 3D textures? Well
		if ((mWidth & mWidth-1) || (mHeight & mHeight-1) || (mDepth & mDepth-1))
			return false;

		if (rkCurCaps.Caps2 & D3DCAPS2_CANAUTOGENMIPMAP)
		{
			HRESULT hr;
			// check for auto gen. mip maps support
			hr = pD3D->CheckDeviceFormat(
					rkCurCaps.AdapterOrdinal, 
					rkCurCaps.DeviceType, 
					eBackBufferFormat, 
					srcUsage | D3DUSAGE_AUTOGENMIPMAP,
					srcType,
					srcFormat);
			// this HR could a SUCCES
			// but mip maps will not be generated
			if (hr == D3D_OK)
				return true;
			else
				return false;
		}
		else
			return false;
	}
	
	D3DFORMAT D3D9Texture::_chooseD3DFormat(IDirect3DDevice9* d3d9Device)
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
	
	// Macro to hide ugly cast
	#define GETLEVEL(face,mip) \
	 	static_cast<D3D9PixelBuffer*>(mSurfaceList[face*(mNumMipmaps+1)+mip].get())
	void D3D9Texture::_createSurfaceList(IDirect3DDevice9* d3d9Device, TextureResources* textureResources)
	{
		IDirect3DSurface9 *surface;
		IDirect3DVolume9 *volume;				
		UINT32 mip, face;

		assert(textureResources != NULL);

		// Need to know static / dynamic
		unsigned int bufusage;
		if ((mUsage & TU_DYNAMIC) && mDynamicTextures)
		{
			bufusage = GBU_DYNAMIC;
		}
		else
		{
			bufusage = GBU_STATIC;
		}
		if (mUsage & TU_RENDERTARGET)
		{
			bufusage |= TU_RENDERTARGET;
		}
		else if(mUsage & TU_DEPTHSTENCIL)
		{
			bufusage |= TU_RENDERTARGET;
		}
		
		UINT32 surfaceCount  = static_cast<UINT32>((getNumFaces() * (mNumMipmaps + 1)));
		bool updateOldList = mSurfaceList.size() == surfaceCount;
		if(!updateOldList)
		{			
			// Create new list of surfaces	
			mSurfaceList.clear();
			for(UINT32 face=0; face<getNumFaces(); ++face)
			{
				for(UINT32 mip=0; mip<=mNumMipmaps; ++mip)
				{
					mSurfaceList.push_back(cm_shared_ptr<D3D9PixelBuffer, PoolAlloc>((GpuBufferUsage)bufusage, this));
				}
			}
		}

		if((mUsage & TU_RENDERTARGET) != 0 && (mFSAAType != D3DMULTISAMPLE_NONE))
		{
			assert(textureResources->pFSAASurface);
			assert(getTextureType() == TEX_TYPE_2D);

			D3D9PixelBuffer* currPixelBuffer = GETLEVEL(0, 0);

			currPixelBuffer->bind(d3d9Device, textureResources->pFSAASurface,
				mHwGammaWriteSupported, mFSAA, "PortNoName", textureResources->pBaseTex);
		}
		else if((mUsage & TU_DEPTHSTENCIL) != 0 && (mFSAAType != D3DMULTISAMPLE_NONE))
		{
			assert(textureResources->pDepthStencilSurface);
			assert(getTextureType() == TEX_TYPE_2D);

			D3D9PixelBuffer* currPixelBuffer = GETLEVEL(0, 0);

			currPixelBuffer->bind(d3d9Device, textureResources->pDepthStencilSurface,
				mHwGammaWriteSupported, mFSAA, "PortNoName", textureResources->pBaseTex);
		}
		else
		{
			assert(textureResources->pBaseTex);
			// Make sure number of mips is right
			UINT32 numCreatedMips = textureResources->pBaseTex->GetLevelCount() - 1;

			if(numCreatedMips != mNumMipmaps)
			{
				CM_EXCEPT(InternalErrorException, "Number of created and wanted mip map levels doesn't match: " + 
					toString(numCreatedMips) + "/" + toString(mNumMipmaps));
			}

			switch(getTextureType()) 
			{
			case TEX_TYPE_2D:
			case TEX_TYPE_1D:
				assert(textureResources->pNormTex);
				// For all mipmaps, store surfaces as HardwarePixelBufferPtr
				for(mip=0; mip<=mNumMipmaps; ++mip)
				{
					if(textureResources->pNormTex->GetSurfaceLevel(static_cast<UINT>(mip), &surface) != D3D_OK)
						CM_EXCEPT(RenderingAPIException, "Get surface level failed");

					D3D9PixelBuffer* currPixelBuffer = GETLEVEL(0, mip);

					currPixelBuffer->bind(d3d9Device, surface,
						mHwGammaWriteSupported, mFSAA, "PortNoName", textureResources->pBaseTex);

					// decrement reference count, the GetSurfaceLevel call increments this
					// this is safe because the pixel buffer keeps a reference as well
					surface->Release();			
				}

				break;
			case TEX_TYPE_CUBE_MAP:
				assert(textureResources->pCubeTex);
				// For all faces and mipmaps, store surfaces as HardwarePixelBufferPtr
				for(face=0; face<6; ++face)
				{
					for(mip=0; mip<=mNumMipmaps; ++mip)
					{
						if(textureResources->pCubeTex->GetCubeMapSurface((D3DCUBEMAP_FACES)face, static_cast<UINT>(mip), &surface) != D3D_OK)
							CM_EXCEPT(RenderingAPIException, "Get cubemap surface failed");

						D3D9PixelBuffer* currPixelBuffer = GETLEVEL(face, mip);

						currPixelBuffer->bind(d3d9Device, surface,
							mHwGammaWriteSupported, mFSAA, "NoNamePort", textureResources->pBaseTex);

						// decrement reference count, the GetSurfaceLevel call increments this
						// this is safe because the pixel buffer keeps a reference as well
						surface->Release();				
					}				
				}
				break;
			case TEX_TYPE_3D:
				assert(textureResources->pVolumeTex);
				// For all mipmaps, store surfaces as HardwarePixelBufferPtr
				for(mip=0; mip<=mNumMipmaps; ++mip)
				{
					if(textureResources->pVolumeTex->GetVolumeLevel(static_cast<UINT>(mip), &volume) != D3D_OK)
						CM_EXCEPT(RenderingAPIException, "Get volume level failed");	

					D3D9PixelBuffer* currPixelBuffer = GETLEVEL(0, mip);

					currPixelBuffer->bind(d3d9Device, volume, textureResources->pBaseTex);

					// decrement reference count, the GetSurfaceLevel call increments this
					// this is safe because the pixel buffer keeps a reference as well
					volume->Release();
				}
				break;
			};		
		}
					
	}
	#undef GETLEVEL
	
	PixelBufferPtr D3D9Texture::getBuffer(UINT32 face, UINT32 mipmap) 
	{
		THROW_IF_NOT_CORE_THREAD;

		if(face >= getNumFaces())
			CM_EXCEPT(InvalidParametersException, "A three dimensional cube has six faces");
		if(mipmap > mNumMipmaps)
			CM_EXCEPT(InvalidParametersException, "Mipmap index out of range");
		UINT32 idx = face*(mNumMipmaps+1) + mipmap;

		IDirect3DDevice9* d3d9Device = D3D9RenderSystem::getActiveD3D9Device();
		TextureResources* textureResources = getTextureResources(d3d9Device);
		if (textureResources == NULL || textureResources->pBaseTex == NULL)
		{				
			createTextureResources(d3d9Device);
			textureResources = getTextureResources(d3d9Device);			
		}
	
		assert(textureResources != NULL);
		assert(idx < mSurfaceList.size());
		return mSurfaceList[idx];
	}
	//---------------------------------------------------------------------
	bool D3D9Texture::useDefaultPool()
	{
		// Determine D3D pool to use
		// Use managed unless we're a render target or user has asked for 
		// a dynamic texture, and device supports D3DUSAGE_DYNAMIC (because default pool
		// resources without the dynamic flag are not lockable)
		return (mUsage & TU_RENDERTARGET) || (mUsage & TU_DEPTHSTENCIL) || ((mUsage & TU_DYNAMIC) && mDynamicTextures);
	}
	
	//---------------------------------------------------------------------
	void D3D9Texture::notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device) 
	{		
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		if (D3D9RenderSystem::getResourceManager()->getCreationPolicy() == RCP_CREATE_ON_ALL_DEVICES)
			createTextureResources(d3d9Device);
	}

	//---------------------------------------------------------------------
	void D3D9Texture::notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device) 
	{				
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		DeviceToTextureResourcesIterator it = mMapDeviceToTextureResources.find(d3d9Device);

		if (it != mMapDeviceToTextureResources.end())
		{			
			TextureResources* textureResource = it->second;

			// Destroy surfaces from each mip level.
			for(unsigned int i = 0; i < mSurfaceList.size(); ++i)
			{
				D3D9PixelBuffer* pixelBuffer = static_cast<D3D9PixelBuffer*>(mSurfaceList[i].get());

				pixelBuffer->destroyBufferResources(d3d9Device);			
			}

			// Just free any internal resources, don't call unload() here
			// because we want the un-touched resource to keep its unloaded status
			// after device reset.
			freeTextureResources(d3d9Device, textureResource);

			if(textureResource != nullptr)
				cm_delete<PoolAlloc>(textureResource);

			mMapDeviceToTextureResources.erase(it);
		}	
	}

	//---------------------------------------------------------------------
	void D3D9Texture::notifyOnDeviceLost(IDirect3DDevice9* d3d9Device) 
	{		
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		if(mD3DPool == D3DPOOL_DEFAULT)
		{
			DeviceToTextureResourcesIterator it = mMapDeviceToTextureResources.find(d3d9Device);

			if (it != mMapDeviceToTextureResources.end())
			{
				TextureResources* textureResource = it->second;				

				// Just free any internal resources, don't call unload() here
				// because we want the un-touched resource to keep its unloaded status
				// after device reset.
				freeTextureResources(d3d9Device, textureResource);
			}					
		}		
	}

	//---------------------------------------------------------------------
	void D3D9Texture::notifyOnDeviceReset(IDirect3DDevice9* d3d9Device) 
	{		
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		if(mD3DPool == D3DPOOL_DEFAULT)
		{			
			createTextureResources(d3d9Device);
		}
	}

	//---------------------------------------------------------------------
	IDirect3DBaseTexture9* D3D9Texture::getTexture_internal()
	{
		THROW_IF_NOT_CORE_THREAD;

		TextureResources* textureResources;			
		IDirect3DDevice9* d3d9Device = D3D9RenderSystem::getActiveD3D9Device();
			
		textureResources = getTextureResources(d3d9Device);		
		if (textureResources == NULL || textureResources->pBaseTex == NULL)
		{			
			createTextureResources(d3d9Device);
			textureResources = getTextureResources(d3d9Device);			
		}
		assert(textureResources); 
		assert(textureResources->pBaseTex); 

		return textureResources->pBaseTex;
	}

	//---------------------------------------------------------------------
	IDirect3DTexture9* D3D9Texture::getNormTexture_internal()
	{
		THROW_IF_NOT_CORE_THREAD;

		TextureResources* textureResources;
		IDirect3DDevice9* d3d9Device = D3D9RenderSystem::getActiveD3D9Device();
		
		textureResources = getTextureResources(d3d9Device);		
		if (textureResources == NULL || textureResources->pNormTex == NULL)
		{
			createTextureResources(d3d9Device);
			textureResources = getTextureResources(d3d9Device);			
		}
		assert(textureResources); 
		assert(textureResources->pNormTex); 

		return textureResources->pNormTex;
	}

	//---------------------------------------------------------------------
	IDirect3DCubeTexture9* D3D9Texture::getCubeTexture_internal()
	{
		THROW_IF_NOT_CORE_THREAD;

		TextureResources* textureResources;
		IDirect3DDevice9* d3d9Device = D3D9RenderSystem::getActiveD3D9Device();
		
		textureResources = getTextureResources(d3d9Device);		
		if (textureResources == NULL || textureResources->pCubeTex)
		{
			createTextureResources(d3d9Device);
			textureResources = getTextureResources(d3d9Device);			
		}
		assert(textureResources); 
		assert(textureResources->pCubeTex); 

		return textureResources->pCubeTex;
	}	
}
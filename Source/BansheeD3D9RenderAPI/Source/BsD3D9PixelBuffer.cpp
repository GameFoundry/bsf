//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsD3D9PixelBuffer.h"
#include "BsD3D9Texture.h"
#include "BsD3D9Mappings.h"
#include "BsException.h"
#include "BsBitwise.h"
#include "BsRenderAPI.h"
#include "BsRenderStats.h"

namespace BansheeEngine 
{
	RECT toD3DRECT(const PixelVolume &lockBox)
	{
		RECT prect;
		assert(lockBox.getDepth() == 1);
		prect.left = (LONG)lockBox.left;
		prect.right = (LONG)lockBox.right;
		prect.top = (LONG)lockBox.top;
		prect.bottom = (LONG)lockBox.bottom;

		return prect;
	}

	D3DBOX toD3DBOX(const PixelVolume &lockBox)
	{
		D3DBOX pbox;

		pbox.Left = (UINT)lockBox.left;
		pbox.Right = (UINT)lockBox.right;
		pbox.Top = (UINT)lockBox.top;
		pbox.Bottom = (UINT)lockBox.bottom;
		pbox.Front = (UINT)lockBox.front;
		pbox.Back = (UINT)lockBox.back;

		return pbox;
	}

	D3D9PixelBuffer::D3D9PixelBuffer(GpuBufferUsage usage, D3D9TextureCore* ownerTexture)
		:PixelBuffer(0, 0, 0, PF_UNKNOWN, usage, false),
		 mDoMipmapGen(0), mHWMipmaps(0), mOwnerTexture(ownerTexture)
	{ }

	D3D9PixelBuffer::~D3D9PixelBuffer()
	{
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION;

		auto iter = mMapDeviceToBufferResources.begin();

		while (iter != mMapDeviceToBufferResources.end())
		{
			SAFE_RELEASE(iter->second->surface);
			SAFE_RELEASE(iter->second->volume);

			if(iter->second != nullptr)
				bs_delete(iter->second);

			auto toRemove = iter++;
			mMapDeviceToBufferResources.erase(toRemove);
		}
	}

	void D3D9PixelBuffer::bind(IDirect3DDevice9* dev, IDirect3DSurface9* surface, IDirect3DBaseTexture9* mipTex)
	{
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		BufferResources* bufferResources = getBufferResources(dev);
		bool isNewBuffer = false;

		if (bufferResources == NULL)
		{
			bufferResources = createBufferResources();		
			mMapDeviceToBufferResources[dev] = bufferResources;
			isNewBuffer = true;
		}
		
		bufferResources->mipTex = mipTex;
		bufferResources->surface = surface;
		bufferResources->surface->AddRef();

		D3DSURFACE_DESC desc;
		if(surface->GetDesc(&desc) != D3D_OK)
			BS_EXCEPT(RenderingAPIException, "Could not get surface information");

		mWidth = desc.Width;
		mHeight = desc.Height;
		mDepth = 1;
		mFormat = D3D9Mappings::_getPF(desc.Format);

		mRowPitch = mWidth;
		mSlicePitch = mHeight*mWidth;
		mSizeInBytes = PixelUtil::getMemorySize(mWidth, mHeight, mDepth, mFormat);	
	}

	void D3D9PixelBuffer::bind(IDirect3DDevice9* dev, IDirect3DVolume9* volume, IDirect3DBaseTexture9* mipTex)
	{
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		BufferResources* bufferResources = getBufferResources(dev);
		bool isNewBuffer = false;

		if (bufferResources == NULL)
		{
			bufferResources = createBufferResources();
			mMapDeviceToBufferResources[dev] = bufferResources;
			isNewBuffer = true;
		}

		bufferResources->mipTex = mipTex;
		bufferResources->volume = volume;
		bufferResources->volume->AddRef();
	
		D3DVOLUME_DESC desc;
		if(volume->GetDesc(&desc) != D3D_OK)
			BS_EXCEPT(RenderingAPIException, "Could not get volume information");

		mWidth = desc.Width;
		mHeight = desc.Height;
		mDepth = desc.Depth;
		mFormat = D3D9Mappings::_getPF(desc.Format);

		mRowPitch = mWidth;
		mSlicePitch = mHeight*mWidth;
		mSizeInBytes = PixelUtil::getMemorySize(mWidth, mHeight, mDepth, mFormat);
	}

	D3D9PixelBuffer::BufferResources* D3D9PixelBuffer::getBufferResources(IDirect3DDevice9* d3d9Device)
	{
		auto iterFind = mMapDeviceToBufferResources.find(d3d9Device);

		if (iterFind != mMapDeviceToBufferResources.end())	
			return iterFind->second;
	
		return nullptr;
	}

	D3D9PixelBuffer::BufferResources* D3D9PixelBuffer::createBufferResources()
	{
		BufferResources* newResources = bs_new<BufferResources>();
		memset(newResources, 0, sizeof(BufferResources));

		return newResources;
	}

	void D3D9PixelBuffer::destroyBufferResources(IDirect3DDevice9* d3d9Device)
	{
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		auto iterFind = mMapDeviceToBufferResources.find(d3d9Device);

		if (iterFind != mMapDeviceToBufferResources.end())
		{
			SAFE_RELEASE(iterFind->second->surface);
			SAFE_RELEASE(iterFind->second->volume);	

			if(iterFind->second != nullptr)
				bs_delete(iterFind->second);

			mMapDeviceToBufferResources.erase(iterFind);
		}
	}

	void D3D9PixelBuffer::lockDeviceAccess()
	{
		D3D9_DEVICE_ACCESS_LOCK;			
	}

	void D3D9PixelBuffer::unlockDeviceAccess()
	{
		D3D9_DEVICE_ACCESS_UNLOCK;								
	}

	PixelData D3D9PixelBuffer::lockImpl(PixelVolume lockBox, GpuLockOptions options)
	{	
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

#if BS_PROFILING_ENABLED
		if (options == GBL_READ_ONLY || options == GBL_READ_WRITE)
		{
			BS_INC_RENDER_STAT_CAT(ResRead, RenderStatObject_Texture);
		}

		if (options == GBL_READ_WRITE || options == GBL_WRITE_ONLY || options == GBL_WRITE_ONLY_DISCARD || options == GBL_WRITE_ONLY_NO_OVERWRITE)
		{
			BS_INC_RENDER_STAT_CAT(ResWrite, RenderStatObject_Texture);
		}
#endif

		DWORD flags = 0;
		switch(options)
		{
		case GBL_WRITE_ONLY_DISCARD:
			if (mUsage & GBU_DYNAMIC)
				flags |= D3DLOCK_DISCARD;
			break;
		case GBL_READ_ONLY:
			flags |= D3DLOCK_READONLY;
			break;
		default: 
			break;
		};

		if (mMapDeviceToBufferResources.size() == 0)
		{
			BS_EXCEPT(RenderingAPIException, "There are no resources attached to this pixel buffer !!");	
		}
	
		mLockedBox = lockBox;
		mLockFlags = flags;

		BufferResources* bufferResources = mMapDeviceToBufferResources.begin()->second;
	
		return lockBuffer(bufferResources, lockBox, flags);
	}

	PixelData D3D9PixelBuffer::lockBuffer(BufferResources* bufferResources, const PixelVolume& lockBox, DWORD flags)
	{
		PixelData rval(lockBox.getWidth(), lockBox.getHeight(), lockBox.getDepth(), mFormat);

		if (bufferResources->surface != nullptr)
		{
			D3DLOCKED_RECT lrect;
			HRESULT hr;

			if (lockBox.left == 0 && lockBox.top == 0 && lockBox.right == mWidth && lockBox.bottom == mHeight)
			{
				hr = bufferResources->surface->LockRect(&lrect, nullptr, flags);
			}
			else
			{
				RECT prect = toD3DRECT(lockBox);
				hr = bufferResources->surface->LockRect(&lrect, &prect, flags);
			}

			if (FAILED(hr))		
				BS_EXCEPT(RenderingAPIException, "Surface locking failed");

			initPixelDataFromD3DLock(rval, lrect);
		} 
		else if(bufferResources->volume) 
		{
			D3DBOX pbox = toD3DBOX(lockBox);
			D3DLOCKED_BOX lbox;

			if(bufferResources->volume->LockBox(&lbox, &pbox, flags) != D3D_OK)
				BS_EXCEPT(RenderingAPIException, "Volume locking failed");

			initPixelDataFromD3DLock(rval, lbox);
		}

		return rval;
	}

	void D3D9PixelBuffer::unlockImpl()
	{
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		if (mMapDeviceToBufferResources.size() == 0)
			BS_EXCEPT(RenderingAPIException, "There are no resources attached to this pixel buffer.");	

		auto it = mMapDeviceToBufferResources.begin();							
		unlockBuffer(it->second);	

		if(mDoMipmapGen)
			genMipmaps(it->second->mipTex);	
	}

	void D3D9PixelBuffer::unlockBuffer(BufferResources* bufferResources)
	{
		if(bufferResources->surface) 
		{
			bufferResources->surface->UnlockRect();
		} 
		else if(bufferResources->volume) 
		{
			bufferResources->volume->UnlockBox();
		}
	}

	void D3D9PixelBuffer::genMipmaps(IDirect3DBaseTexture9* mipTex)
	{
		assert(mipTex != nullptr);

		if (mHWMipmaps)
		{
			mipTex->GenerateMipSubLevels();
		}
		else
		{
			if(D3DXFilterTexture(mipTex, nullptr, D3DX_DEFAULT, D3DX_DEFAULT) != D3D_OK)
			{
				BS_EXCEPT(RenderingAPIException, "Failed to generate mipmaps.");
			}
		}
	}

	void D3D9PixelBuffer::setMipmapping(bool doMipmapGen, bool HWMipmaps)
	{	
		mDoMipmapGen = doMipmapGen;
		mHWMipmaps = HWMipmaps;	
	}

	void D3D9PixelBuffer::releaseSurfaces(IDirect3DDevice9* d3d9Device)
	{
		BufferResources* bufferResources = getBufferResources(d3d9Device);

		if (bufferResources != nullptr)
		{
			SAFE_RELEASE(bufferResources->surface);
			SAFE_RELEASE(bufferResources->volume);
		}
	}

	IDirect3DSurface9* D3D9PixelBuffer::getSurface(IDirect3DDevice9* d3d9Device)
	{
		BufferResources* bufferResources = getBufferResources(d3d9Device);

		if (bufferResources	== nullptr)
		{
			mOwnerTexture->createInternalResources(d3d9Device);
			bufferResources = getBufferResources(d3d9Device);
		}

		return bufferResources->surface;
	}

	void D3D9PixelBuffer::initPixelDataFromD3DLock(PixelData& rval, const D3DLOCKED_RECT& lrect)
	{
		UINT32 bpp = PixelUtil::getNumElemBytes(rval.getFormat());
		if (bpp != 0)
		{
			rval.setRowPitch(lrect.Pitch / bpp);
			rval.setSlicePitch(rval.getRowPitch() * rval.getHeight());
			assert((lrect.Pitch % bpp) == 0);
		}
		else if (PixelUtil::isCompressed(rval.getFormat()))
		{
			rval.setRowPitch(rval.getWidth());
			rval.setSlicePitch(rval.getWidth() * rval.getHeight());
		}
		else
		{
			BS_EXCEPT(InvalidParametersException, "Invalid pixel format.");
		}

		rval.setExternalBuffer((UINT8*)lrect.pBits);
	}

	void D3D9PixelBuffer::initPixelDataFromD3DLock(PixelData& rval, const D3DLOCKED_BOX& lbox)
	{
		UINT32 bpp = PixelUtil::getNumElemBytes(rval.getFormat());
		if (bpp != 0)
		{
			rval.setRowPitch(lbox.RowPitch / bpp);
			rval.setSlicePitch(lbox.SlicePitch / bpp);
			assert((lbox.RowPitch % bpp) == 0);
			assert((lbox.SlicePitch % bpp) == 0);
		}
		else if (PixelUtil::isCompressed(rval.getFormat()))
		{
			rval.setRowPitch(rval.getWidth());
			rval.setSlicePitch(rval.getWidth() * rval.getHeight());
		}
		else
		{
			BS_EXCEPT(InvalidParametersException, "Invalid pixel format.");
		}

		rval.setExternalBuffer((UINT8*)lbox.pBits);
	}
};

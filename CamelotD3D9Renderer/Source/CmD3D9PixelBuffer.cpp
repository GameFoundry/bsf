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
#include "CmD3D9PixelBuffer.h"
#include "CmD3D9Texture.h"
#include "CmD3D9Mappings.h"
#include "CmException.h"
#include "CmBitwise.h"
#include "CmRenderSystem.h"

namespace BansheeEngine 
{
	D3D9PixelBuffer::D3D9PixelBuffer(GpuBufferUsage usage, D3D9Texture* ownerTexture)
		:PixelBuffer(0, 0, 0, PF_UNKNOWN, usage, false),
		 mDoMipmapGen(0), mHWMipmaps(0), mOwnerTexture(ownerTexture)
	{ }

	D3D9PixelBuffer::~D3D9PixelBuffer()
	{
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		DeviceToBufferResourcesIterator it = mMapDeviceToBufferResources.begin();

		while (it != mMapDeviceToBufferResources.end())
		{
			SAFE_RELEASE(it->second->surface);
			SAFE_RELEASE(it->second->volume);

			if(it->second != nullptr)
				cm_delete<PoolAlloc>(it->second);

			DeviceToBufferResourcesIterator deadi = it++;
			mMapDeviceToBufferResources.erase(deadi);
		}
	}
	//-----------------------------------------------------------------------------  
	void D3D9PixelBuffer::bind(IDirect3DDevice9 *dev, IDirect3DSurface9 *surface, 
									   bool writeGamma, UINT32 fsaa, const String& srcName,
									   IDirect3DBaseTexture9 *mipTex)
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
			CM_EXCEPT(RenderingAPIException, "Could not get surface information");

		mWidth = desc.Width;
		mHeight = desc.Height;
		mDepth = 1;
		mFormat = D3D9Mappings::_getPF(desc.Format);
		// Default
		mRowPitch = mWidth;
		mSlicePitch = mHeight*mWidth;
		mSizeInBytes = PixelUtil::getMemorySize(mWidth, mHeight, mDepth, mFormat);	
	}
	//-----------------------------------------------------------------------------
	void D3D9PixelBuffer::bind(IDirect3DDevice9 *dev, IDirect3DVolume9 *volume, IDirect3DBaseTexture9 *mipTex)
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
			CM_EXCEPT(RenderingAPIException, "Could not get volume information");
		mWidth = desc.Width;
		mHeight = desc.Height;
		mDepth = desc.Depth;
		mFormat = D3D9Mappings::_getPF(desc.Format);
		// Default
		mRowPitch = mWidth;
		mSlicePitch = mHeight*mWidth;
		mSizeInBytes = PixelUtil::getMemorySize(mWidth, mHeight, mDepth, mFormat);
	}

	//-----------------------------------------------------------------------------  
	D3D9PixelBuffer::BufferResources* D3D9PixelBuffer::getBufferResources(IDirect3DDevice9* d3d9Device)
	{
		DeviceToBufferResourcesIterator it = mMapDeviceToBufferResources.find(d3d9Device);

		if (it != mMapDeviceToBufferResources.end())	
			return it->second;
	
		return NULL;
	}

	//-----------------------------------------------------------------------------  
	D3D9PixelBuffer::BufferResources* D3D9PixelBuffer::createBufferResources()
	{
		BufferResources* newResources = cm_new<BufferResources, PoolAlloc>();

		memset(newResources, 0, sizeof(BufferResources));

		return newResources;
	}

	//-----------------------------------------------------------------------------  
	void D3D9PixelBuffer::destroyBufferResources(IDirect3DDevice9* d3d9Device)
	{
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		DeviceToBufferResourcesIterator it = mMapDeviceToBufferResources.find(d3d9Device);

		if (it != mMapDeviceToBufferResources.end())
		{
			SAFE_RELEASE(it->second->surface);
			SAFE_RELEASE(it->second->volume);	

			if(it->second != nullptr)
				cm_delete<PoolAlloc>(it->second);

			mMapDeviceToBufferResources.erase(it);
		}
	}

	//-----------------------------------------------------------------------------  
	void D3D9PixelBuffer::lockDeviceAccess()
	{
		D3D9_DEVICE_ACCESS_LOCK;			
	}

	//-----------------------------------------------------------------------------  
	void D3D9PixelBuffer::unlockDeviceAccess()
	{
		D3D9_DEVICE_ACCESS_UNLOCK;								
	}

	//-----------------------------------------------------------------------------  
	// Util functions to convert a D3D locked box to a pixel box
	void fromD3DLock(PixelData &rval, const D3DLOCKED_RECT &lrect)
	{
		UINT32 bpp = PixelUtil::getNumElemBytes(rval.getFormat());
		if (bpp != 0)
		{
			rval.setRowPitch(lrect.Pitch / bpp);
			rval.setSlicePitch(rval.getRowPitch() * rval.getHeight());
			assert((lrect.Pitch % bpp)==0);
		}
		else if (PixelUtil::isCompressed(rval.getFormat()))
		{
			rval.setRowPitch(rval.getWidth());
			rval.setSlicePitch(rval.getWidth() * rval.getHeight());
		}
		else
		{
			CM_EXCEPT(InvalidParametersException, "Invalid pixel format");
		}

		rval.setExternalBuffer((UINT8*)lrect.pBits);
	}
	void fromD3DLock(PixelData &rval, const D3DLOCKED_BOX &lbox)
	{
		UINT32 bpp = PixelUtil::getNumElemBytes(rval.getFormat());
		if (bpp != 0)
		{
			rval.setRowPitch(lbox.RowPitch / bpp);
			rval.setSlicePitch(lbox.SlicePitch / bpp);
			assert((lbox.RowPitch % bpp)==0);
			assert((lbox.SlicePitch % bpp)==0);
		}
		else if (PixelUtil::isCompressed(rval.getFormat()))
		{
			rval.setRowPitch(rval.getWidth());
			rval.setSlicePitch(rval.getWidth() * rval.getHeight());
		}
		else
		{
			CM_EXCEPT(InvalidParametersException, "Invalid pixel format");
		}
		rval.setExternalBuffer((UINT8*)lbox.pBits);
	}
	// Convert Ogre integer Box to D3D rectangle
	RECT toD3DRECT(const PixelVolume &lockBox)
	{
		RECT prect;
		assert(lockBox.getDepth() == 1);
		prect.left = static_cast<LONG>(lockBox.left);
		prect.right = static_cast<LONG>(lockBox.right);
		prect.top = static_cast<LONG>(lockBox.top);
		prect.bottom = static_cast<LONG>(lockBox.bottom);
		return prect;
	}
	// Convert Ogre integer Box to D3D box
	D3DBOX toD3DBOX(const PixelVolume &lockBox)
	{
		D3DBOX pbox;
	
		pbox.Left = static_cast<UINT>(lockBox.left);
		pbox.Right = static_cast<UINT>(lockBox.right);
		pbox.Top = static_cast<UINT>(lockBox.top);
		pbox.Bottom = static_cast<UINT>(lockBox.bottom);
		pbox.Front = static_cast<UINT>(lockBox.front);
		pbox.Back = static_cast<UINT>(lockBox.back);
		return pbox;
	}
	// Convert Ogre pixelbox extent to D3D rectangle
	RECT toD3DRECTExtent(const PixelData &lockBox)
	{
		RECT prect;
		assert(lockBox.getDepth() == 1);
		prect.left = 0;
		prect.right = static_cast<LONG>(lockBox.getWidth());
		prect.top = 0;
		prect.bottom = static_cast<LONG>(lockBox.getHeight());
		return prect;
	}
	// Convert Ogre pixelbox extent to D3D box
	D3DBOX toD3DBOXExtent(const PixelData &lockBox)
	{
		D3DBOX pbox;
		pbox.Left = 0;
		pbox.Right = static_cast<UINT>(lockBox.getWidth());
		pbox.Top = 0;
		pbox.Bottom = static_cast<UINT>(lockBox.getHeight());
		pbox.Front = 0;
		pbox.Back = static_cast<UINT>(lockBox.getDepth());
		return pbox;
	}
	//-----------------------------------------------------------------------------  
	PixelData D3D9PixelBuffer::lockImpl(const PixelVolume lockBox,  GpuLockOptions options)
	{	
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		// Set locking flags according to options
		DWORD flags = 0;
		switch(options)
		{
		case GBL_WRITE_ONLY_DISCARD:
			// D3D only likes D3DLOCK_DISCARD if you created the texture with D3DUSAGE_DYNAMIC
			// debug runtime flags this up, could cause problems on some drivers
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
			CM_EXCEPT(RenderingAPIException, "There are no resources attached to this pixel buffer !!");	
		}
	
		mLockedBox = lockBox;
		mLockFlags = flags;

		BufferResources* bufferResources = mMapDeviceToBufferResources.begin()->second;
	
		// Lock the source buffer.
		return lockBuffer(bufferResources, lockBox, flags);
	}

	//-----------------------------------------------------------------------------  
	BansheeEngine::PixelData D3D9PixelBuffer::lockBuffer(BufferResources* bufferResources, 
													   const PixelVolume &lockBox, 
													   DWORD flags)
	{
		// Set extents and format
		// Note that we do not carry over the left/top/front here, since the returned
		// PixelBox will be re-based from the locking point onwards
		PixelData rval(lockBox.getWidth(), lockBox.getHeight(), lockBox.getDepth(), mFormat);

		if (bufferResources->surface != NULL) 
		{
			// Surface
			D3DLOCKED_RECT lrect; // Filled in by D3D
			HRESULT hr;

			if (lockBox.left == 0 && lockBox.top == 0 
				&& lockBox.right == mWidth && lockBox.bottom == mHeight)
			{
				// Lock whole surface
				hr = bufferResources->surface->LockRect(&lrect, NULL, flags);
			}
			else
			{
				RECT prect = toD3DRECT(lockBox); // specify range to lock
				hr = bufferResources->surface->LockRect(&lrect, &prect, flags);
			}
			if (FAILED(hr))		
				CM_EXCEPT(RenderingAPIException, "Surface locking failed");
			fromD3DLock(rval, lrect);
		} 
		else if(bufferResources->volume) 
		{
			// Volume
			D3DBOX pbox = toD3DBOX(lockBox); // specify range to lock
			D3DLOCKED_BOX lbox; // Filled in by D3D

			if(bufferResources->volume->LockBox(&lbox, &pbox, flags) != D3D_OK)
				CM_EXCEPT(RenderingAPIException, "Volume locking failed");
			fromD3DLock(rval, lbox);
		}


		return rval;
	}

	//-----------------------------------------------------------------------------  
	void D3D9PixelBuffer::unlockImpl(void)
	{
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		if (mMapDeviceToBufferResources.size() == 0)
		{
			CM_EXCEPT(RenderingAPIException, "There are no resources attached to this pixel buffer !!");	
		}

		DeviceToBufferResourcesIterator it = mMapDeviceToBufferResources.begin();							
		unlockBuffer( it->second);		
		if(mDoMipmapGen)
			_genMipmaps(it->second->mipTex);	
	}

	//-----------------------------------------------------------------------------  
	void D3D9PixelBuffer::unlockBuffer(BufferResources* bufferResources)
	{
		if(bufferResources->surface) 
		{
			// Surface
			bufferResources->surface->UnlockRect();
		} 
		else if(bufferResources->volume) 
		{
			// Volume
			bufferResources->volume->UnlockBox();
		}
	}
	//-----------------------------------------------------------------------------  
	void D3D9PixelBuffer::_genMipmaps(IDirect3DBaseTexture9* mipTex)
	{
		assert(mipTex);

		// Mipmapping
		if (mHWMipmaps)
		{
			// Hardware mipmaps
			mipTex->GenerateMipSubLevels();
		}
		else
		{
			// Software mipmaps
			if( D3DXFilterTexture( mipTex, NULL, D3DX_DEFAULT, D3DX_DEFAULT ) != D3D_OK )
			{
				CM_EXCEPT(RenderingAPIException, "Failed to filter texture (generate mipmaps)");
			}
		}

	}
	//----------------------------------------------------------------------------- 
	void D3D9PixelBuffer::_setMipmapping(bool doMipmapGen, 
												 bool HWMipmaps)
	{	
		mDoMipmapGen = doMipmapGen;
		mHWMipmaps = HWMipmaps;	
	}
	//-----------------------------------------------------------------------------  
	void D3D9PixelBuffer::releaseSurfaces(IDirect3DDevice9* d3d9Device)
	{
		BufferResources* bufferResources = getBufferResources(d3d9Device);

		if (bufferResources != NULL)
		{
			SAFE_RELEASE(bufferResources->surface);
			SAFE_RELEASE(bufferResources->volume);
		}
	}
	//-----------------------------------------------------------------------------   
	IDirect3DSurface9* D3D9PixelBuffer::getSurface(IDirect3DDevice9* d3d9Device)
	{
		BufferResources* bufferResources = getBufferResources(d3d9Device);

		if (bufferResources	== NULL)
		{
			mOwnerTexture->createTextureResources(d3d9Device);
			bufferResources = getBufferResources(d3d9Device);
		}

		return bufferResources->surface;
	}
};

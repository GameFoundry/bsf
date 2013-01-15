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

namespace CamelotEngine 
{
	CM_STATIC_MUTEX_INSTANCE(D3D9PixelBuffer::msDeviceAccessMutex)
	//-----------------------------------------------------------------------------  

	D3D9PixelBuffer::D3D9PixelBuffer(GpuBufferUsage usage, 
													 D3D9Texture* ownerTexture):
		PixelBuffer(0, 0, 0, PF_UNKNOWN, usage, false),
		mDoMipmapGen(0), mHWMipmaps(0), mOwnerTexture(ownerTexture)
	{	
	}
	D3D9PixelBuffer::~D3D9PixelBuffer()
	{
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		DeviceToBufferResourcesIterator it = mMapDeviceToBufferResources.begin();

		while (it != mMapDeviceToBufferResources.end())
		{
			SAFE_RELEASE(it->second->surface);
			SAFE_RELEASE(it->second->volume);
			SAFE_DELETE(it->second);
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
	
		// TODO PORT - My Texture doesn't inherit from Resource and doesn't have that method. Not sure why it needs to call it,
		// but since we're not there's potential for trouble here.
		if (isNewBuffer /*&& mOwnerTexture->isManuallyLoaded()*/)
		{
			DeviceToBufferResourcesIterator it = mMapDeviceToBufferResources.begin();

			while (it != mMapDeviceToBufferResources.end())
			{
				if (it->second != bufferResources && 
					it->second->surface != NULL &&
					it->first->TestCooperativeLevel() == D3D_OK &&
					dev->TestCooperativeLevel() == D3D_OK)
				{
					Box fullBufferBox(0,0,0,mWidth,mHeight,mDepth);
					PixelData dstBox(fullBufferBox, mFormat);

					dstBox.data = new char[getSizeInBytes()];
					blitToMemory(fullBufferBox, dstBox, it->second, it->first);
					blitFromMemory(dstBox, fullBufferBox, bufferResources);
					SAFE_DELETE_ARRAY(dstBox.data);
					break;
				}
				++it;			
			}				
		}
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

		// TODO PORT - My Texture doesn't inherit from Resource and doesn't have that method. Not sure why it needs to call it,
		// but since we're not there's potential for trouble here.
		if (isNewBuffer /*&& mOwnerTexture->isManuallyLoaded()*/)
		{
			DeviceToBufferResourcesIterator it = mMapDeviceToBufferResources.begin();
		
			while (it != mMapDeviceToBufferResources.end())
			{
				if (it->second != bufferResources &&
					it->second->volume != NULL &&
					it->first->TestCooperativeLevel() == D3D_OK &&
					dev->TestCooperativeLevel() == D3D_OK)
				{
					Box fullBufferBox(0,0,0,mWidth,mHeight,mDepth);
					PixelData dstBox(fullBufferBox, mFormat);

					dstBox.data = new char[getSizeInBytes()];
					blitToMemory(fullBufferBox, dstBox, it->second, it->first);
					blitFromMemory(dstBox, fullBufferBox, bufferResources);
					SAFE_DELETE(dstBox.data);
					break;
				}
				++it;			
			}				
		}
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
		BufferResources* newResources = new BufferResources;

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
			SAFE_DELETE(it->second);
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
		UINT32 bpp = PixelUtil::getNumElemBytes(rval.format);
		if (bpp != 0)
		{
			rval.rowPitch = lrect.Pitch / bpp;
			rval.slicePitch = rval.rowPitch * rval.getHeight();
			assert((lrect.Pitch % bpp)==0);
		}
		else if (PixelUtil::isCompressed(rval.format))
		{
			rval.rowPitch = rval.getWidth();
			rval.slicePitch = rval.getWidth() * rval.getHeight();
		}
		else
		{
			CM_EXCEPT(InvalidParametersException, "Invalid pixel format");
		}

		rval.data = lrect.pBits;
	}
	void fromD3DLock(PixelData &rval, const D3DLOCKED_BOX &lbox)
	{
		UINT32 bpp = PixelUtil::getNumElemBytes(rval.format);
		if (bpp != 0)
		{
			rval.rowPitch = lbox.RowPitch / bpp;
			rval.slicePitch = lbox.SlicePitch / bpp;
			assert((lbox.RowPitch % bpp)==0);
			assert((lbox.SlicePitch % bpp)==0);
		}
		else if (PixelUtil::isCompressed(rval.format))
		{
			rval.rowPitch = rval.getWidth();
			rval.slicePitch = rval.getWidth() * rval.getHeight();
		}
		else
		{
			CM_EXCEPT(InvalidParametersException, "Invalid pixel format");
		}
		rval.data = lbox.pBits;
	}
	// Convert Ogre integer Box to D3D rectangle
	RECT toD3DRECT(const Box &lockBox)
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
	D3DBOX toD3DBOX(const Box &lockBox)
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
	PixelData D3D9PixelBuffer::lockImpl(const Box lockBox,  GpuLockOptions options)
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
	CamelotEngine::PixelData D3D9PixelBuffer::lockBuffer(BufferResources* bufferResources, 
													   const Box &lockBox, 
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

		DeviceToBufferResourcesIterator it;
	
		// 1. Update duplicates buffers.
		it = mMapDeviceToBufferResources.begin();
		++it;
		while (it != mMapDeviceToBufferResources.end())
		{			
			BufferResources* bufferResources = it->second;
		
			// Update duplicated buffer from the from the locked buffer content.					
			blitFromMemory(mCurrentLock, mLockedBox, bufferResources);										
			++it;			
		}

		// 2. Unlock the locked buffer.
		it = mMapDeviceToBufferResources.begin();							
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
	void D3D9PixelBuffer::blit(const PixelBufferPtr &rsrc, 
									   const Box &srcBox, 
									   const Box &dstBox)
	{
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		D3D9PixelBuffer *src = static_cast<D3D9PixelBuffer*>(rsrc.get());
		DeviceToBufferResourcesIterator it = mMapDeviceToBufferResources.begin();

		// Update all the buffer copies.
		while (it != mMapDeviceToBufferResources.end())
		{
			BufferResources* srcBufferResources = src->getBufferResources(it->first);
			BufferResources* dstBufferResources = it->second;

			if (srcBufferResources == NULL)
			{
				CM_EXCEPT(RenderingAPIException, "There are no matching resources attached to the source pixel buffer !!");	
			}

			blit(it->first, rsrc, srcBox, dstBox, srcBufferResources, dstBufferResources);
			++it;
		}
	}

	//-----------------------------------------------------------------------------  
	void D3D9PixelBuffer::blit(IDirect3DDevice9* d3d9Device, 
									   const PixelBufferPtr &rsrc, 
									   const Box &srcBox, 
									   const Box &dstBox,
									   BufferResources* srcBufferResources, 
									   BufferResources* dstBufferResources)
	{
		if(dstBufferResources->surface && srcBufferResources->surface)
		{
			// Surface-to-surface
			RECT dsrcRect = toD3DRECT(srcBox);
			RECT ddestRect = toD3DRECT(dstBox);

			D3DSURFACE_DESC srcDesc;
			if(srcBufferResources->surface->GetDesc(&srcDesc) != D3D_OK)
				CM_EXCEPT(RenderingAPIException, "Could not get surface information");

			// If we're blitting from a RTT, try GetRenderTargetData
			// if we're going to try to use GetRenderTargetData, need to use system mem pool
			bool tryGetRenderTargetData = false;
			if ((srcDesc.Usage & D3DUSAGE_RENDERTARGET) != 0
				&& srcDesc.MultiSampleType == D3DMULTISAMPLE_NONE)
			{

				// Temp texture
				IDirect3DTexture9 *tmptex;
				IDirect3DSurface9 *tmpsurface;

				if(D3DXCreateTexture(
					d3d9Device,
					srcDesc.Width, srcDesc.Height,
					1, // 1 mip level ie topmost, generate no mipmaps
					0, srcDesc.Format, D3DPOOL_SYSTEMMEM,
					&tmptex
					) != D3D_OK)
				{
					CM_EXCEPT(RenderingAPIException, "Create temporary texture failed");
				}
				if(tmptex->GetSurfaceLevel(0, &tmpsurface) != D3D_OK)
				{
					tmptex->Release();
					CM_EXCEPT(RenderingAPIException, "Get surface level failed");
				}
				if(d3d9Device->GetRenderTargetData(srcBufferResources->surface, tmpsurface) == D3D_OK)
				{
					// Hey, it worked
					// Copy from this surface instead
					if(D3DXLoadSurfaceFromSurface(
						dstBufferResources->surface, NULL, &ddestRect, 
						tmpsurface, NULL, &dsrcRect,
						D3DX_DEFAULT, 0) != D3D_OK)
					{
						tmpsurface->Release();
						tmptex->Release();
						CM_EXCEPT(RenderingAPIException, "D3DXLoadSurfaceFromSurface failed");
					}
					tmpsurface->Release();
					tmptex->Release();
					return;
				}

			}

			// Otherwise, try the normal method

			// D3DXLoadSurfaceFromSurface
			if(D3DXLoadSurfaceFromSurface(
				dstBufferResources->surface, NULL, &ddestRect, 
				srcBufferResources->surface, NULL, &dsrcRect,
				D3DX_DEFAULT, 0) != D3D_OK)
			{
				CM_EXCEPT(RenderingAPIException, "D3DXLoadSurfaceFromSurface failed");
			}
		}
		else if(dstBufferResources->volume && srcBufferResources->volume)
		{
			// Volume-to-volume
			D3DBOX dsrcBox = toD3DBOX(srcBox);
			D3DBOX ddestBox = toD3DBOX(dstBox);

			// D3DXLoadVolumeFromVolume
			if(D3DXLoadVolumeFromVolume(
				dstBufferResources->volume, NULL, &ddestBox, 
				srcBufferResources->volume, NULL, &dsrcBox,
				D3DX_DEFAULT, 0) != D3D_OK)
			{
				CM_EXCEPT(RenderingAPIException, "D3DXLoadVolumeFromVolume failed");
			}
		}
		else
		{
			// Software fallback   
			PixelBuffer::blit(rsrc, srcBox, dstBox);
		}
	}

	//-----------------------------------------------------------------------------  
	void D3D9PixelBuffer::blitFromMemory(const PixelData &src, const Box &dstBox)
	{	
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		DeviceToBufferResourcesIterator it = mMapDeviceToBufferResources.begin();

		while (it != mMapDeviceToBufferResources.end())
		{		
			BufferResources* dstBufferResources = it->second;
		
			blitFromMemory(src, dstBox, dstBufferResources);	
			++it;
		}
	}

	//-----------------------------------------------------------------------------  
	void D3D9PixelBuffer::blitFromMemory(const PixelData &src, const Box &dstBox, BufferResources* dstBufferResources)
	{
		// for scoped deletion of conversion buffer
		void* data = NULL;
		PixelData converted = src;

		// convert to pixelbuffer's native format if necessary
		if (D3D9Mappings::_getPF(src.format) == D3DFMT_UNKNOWN)
		{
			data = new void*[PixelUtil::getMemorySize(src.getWidth(), src.getHeight(), src.getDepth(), mFormat)];
			converted = PixelData(src.getWidth(), src.getHeight(), src.getDepth(), mFormat, data);
			PixelUtil::bulkPixelConversion(src, converted);
		}

		UINT32 rowWidth;
		if (PixelUtil::isCompressed(converted.format))
		{
			// D3D wants the width of one row of cells in bytes
			if (converted.format == PF_DXT1)
			{
				// 64 bits (8 bytes) per 4x4 block
				rowWidth = (converted.rowPitch / 4) * 8;
			}
			else
			{
				// 128 bits (16 bytes) per 4x4 block
				rowWidth = (converted.rowPitch / 4) * 16;
			}

		}
		else
		{
			rowWidth = converted.rowPitch * PixelUtil::getNumElemBytes(converted.format);
		}

		if (dstBufferResources->surface)
		{
			RECT destRect, srcRect;
			srcRect = toD3DRECT(converted);
			destRect = toD3DRECT(dstBox);

			if(D3DXLoadSurfaceFromMemory(dstBufferResources->surface, NULL, &destRect, 
				converted.data, D3D9Mappings::_getPF(converted.format),
				static_cast<UINT>(rowWidth),
				NULL, &srcRect, D3DX_DEFAULT, 0) != D3D_OK)
			{
				CM_EXCEPT(RenderingAPIException, "D3DXLoadSurfaceFromMemory failed");
			}
		}
		else if (dstBufferResources->volume)
		{
			D3DBOX destBox, srcBox;
			srcBox = toD3DBOX(converted);
			destBox = toD3DBOX(dstBox);
			UINT32 sliceWidth;
			if (PixelUtil::isCompressed(converted.format))
			{
				// D3D wants the width of one slice of cells in bytes
				if (converted.format == PF_DXT1)
				{
					// 64 bits (8 bytes) per 4x4 block
					sliceWidth = (converted.slicePitch / 16) * 8;
				}
				else
				{
					// 128 bits (16 bytes) per 4x4 block
					sliceWidth = (converted.slicePitch / 16) * 16;
				}

			}
			else
			{
				sliceWidth = converted.slicePitch * PixelUtil::getNumElemBytes(converted.format);
			}

			if(D3DXLoadVolumeFromMemory(dstBufferResources->volume, NULL, &destBox, 
				converted.data, D3D9Mappings::_getPF(converted.format),
				static_cast<UINT>(rowWidth), static_cast<UINT>(sliceWidth),
				NULL, &srcBox, D3DX_DEFAULT, 0) != D3D_OK)
			{
				CM_EXCEPT(RenderingAPIException, "D3DXLoadSurfaceFromMemory failed");
			}
		}

		if(mDoMipmapGen)
			_genMipmaps(dstBufferResources->mipTex);

		if(data != NULL)
			delete[] data;
	}

	//-----------------------------------------------------------------------------  
	void D3D9PixelBuffer::blitToMemory(const Box &srcBox, const PixelData &dst)
	{
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		DeviceToBufferResourcesIterator it = mMapDeviceToBufferResources.begin();
		BufferResources* bufferResources = it->second;

		blitToMemory(srcBox, dst, bufferResources, it->first);
	}

	//-----------------------------------------------------------------------------  
	void D3D9PixelBuffer::blitToMemory(const Box &srcBox, const PixelData &dst, 
											   BufferResources* srcBufferResources,
											   IDirect3DDevice9* d3d9Device)
	{
		// Decide on pixel format of temp surface
		PixelFormat tmpFormat = mFormat; 
		if(D3D9Mappings::_getPF(dst.format) != D3DFMT_UNKNOWN)
		{
			tmpFormat = dst.format;
		}

		if (srcBufferResources->surface)
		{
			assert(srcBox.getDepth() == 1 && dst.getDepth() == 1);
			// Create temp texture
			IDirect3DTexture9 *tmp;
			IDirect3DSurface9 *surface;

			D3DSURFACE_DESC srcDesc;
			if(srcBufferResources->surface->GetDesc(&srcDesc) != D3D_OK)
				CM_EXCEPT(RenderingAPIException, "Could not get surface information");

			D3DPOOL temppool = D3DPOOL_SCRATCH;
			// if we're going to try to use GetRenderTargetData, need to use system mem pool
			bool tryGetRenderTargetData = false;
			if (((srcDesc.Usage & D3DUSAGE_RENDERTARGET) != 0) &&
				(srcBox.getWidth() == dst.getWidth()) && (srcBox.getHeight() == dst.getHeight()) &&
				(srcBox.getWidth() == getWidth()) && (srcBox.getHeight() == getHeight()) &&
				(mFormat == tmpFormat))
			{
				tryGetRenderTargetData = true;
				temppool = D3DPOOL_SYSTEMMEM;
			}

			if(D3DXCreateTexture(
				d3d9Device,
				static_cast<UINT>(dst.getWidth()), static_cast<UINT>(dst.getHeight()), 
				1, // 1 mip level ie topmost, generate no mipmaps
				0, D3D9Mappings::_getPF(tmpFormat), temppool,
				&tmp
				) != D3D_OK)
			{
				CM_EXCEPT(RenderingAPIException, "Create temporary texture failed");
			}
			if(tmp->GetSurfaceLevel(0, &surface) != D3D_OK)
			{
				tmp->Release();
				CM_EXCEPT(RenderingAPIException, "Get surface level failed");
			}
			// Copy texture to this temp surface
			RECT destRect, srcRect;
			srcRect = toD3DRECT(srcBox);
			destRect = toD3DRECTExtent(dst);

			// Get the real temp surface format
			D3DSURFACE_DESC dstDesc;
			if(surface->GetDesc(&dstDesc) != D3D_OK)
				CM_EXCEPT(RenderingAPIException, "Could not get surface information");
			tmpFormat = D3D9Mappings::_getPF(dstDesc.Format);

			// Use fast GetRenderTargetData if we are in its usage conditions
			bool fastLoadSuccess = false;
			if (tryGetRenderTargetData)
			{
				if(d3d9Device->GetRenderTargetData(srcBufferResources->surface, surface) == D3D_OK)
				{
					fastLoadSuccess = true;
				}
			}
			if (!fastLoadSuccess)
			{
				if(D3DXLoadSurfaceFromSurface(
					surface, NULL, &destRect, 
					srcBufferResources->surface, NULL, &srcRect,
					D3DX_DEFAULT, 0) != D3D_OK)
				{
					surface->Release();
					tmp->Release();
					CM_EXCEPT(RenderingAPIException, "D3DXLoadSurfaceFromSurface failed");
				}
			}

			// Lock temp surface and copy it to memory
			D3DLOCKED_RECT lrect; // Filled in by D3D
			if(surface->LockRect(&lrect, NULL,  D3DLOCK_READONLY) != D3D_OK)
			{
				surface->Release();
				tmp->Release();
				CM_EXCEPT(RenderingAPIException, "surface->LockRect");
			}
			// Copy it
			PixelData locked(dst.getWidth(), dst.getHeight(), dst.getDepth(), tmpFormat);
			fromD3DLock(locked, lrect);
			PixelUtil::bulkPixelConversion(locked, dst);
			surface->UnlockRect();
			// Release temporary surface and texture
			surface->Release();
			tmp->Release();
		}
		else if (srcBufferResources->volume)
		{
			// Create temp texture
			IDirect3DVolumeTexture9 *tmp;
			IDirect3DVolume9 *surface;

			if(D3DXCreateVolumeTexture(
				d3d9Device,
				static_cast<UINT>(dst.getWidth()), 
				static_cast<UINT>(dst.getHeight()), 
				static_cast<UINT>(dst.getDepth()), 0,
				0, D3D9Mappings::_getPF(tmpFormat), D3DPOOL_SCRATCH,
				&tmp
				) != D3D_OK)
			{
				CM_EXCEPT(RenderingAPIException, "Create temporary texture failed");
			}
			if(tmp->GetVolumeLevel(0, &surface) != D3D_OK)
			{
				tmp->Release();
				CM_EXCEPT(RenderingAPIException, "Get volume level failed");
			}
			// Volume
			D3DBOX ddestBox, dsrcBox;
			ddestBox = toD3DBOXExtent(dst);
			dsrcBox = toD3DBOX(srcBox);

			if(D3DXLoadVolumeFromVolume(
				surface, NULL, &ddestBox, 
				srcBufferResources->volume, NULL, &dsrcBox,
				D3DX_DEFAULT, 0) != D3D_OK)
			{
				surface->Release();
				tmp->Release();
				CM_EXCEPT(RenderingAPIException, "D3DXLoadVolumeFromVolume failed");
			}
			// Lock temp surface and copy it to memory
			D3DLOCKED_BOX lbox; // Filled in by D3D
			if(surface->LockBox(&lbox, NULL,  D3DLOCK_READONLY) != D3D_OK)
			{
				surface->Release();
				tmp->Release();
				CM_EXCEPT(RenderingAPIException, "surface->LockBox");
			}
			// Copy it
			PixelData locked(dst.getWidth(), dst.getHeight(), dst.getDepth(), tmpFormat);
			fromD3DLock(locked, lbox);
			PixelUtil::bulkPixelConversion(locked, dst);
			surface->UnlockBox();
			// Release temporary surface and texture
			surface->Release();
			tmp->Release();
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

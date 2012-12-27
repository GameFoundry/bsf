#include "CmD3D11HardwarePixelBuffer.h"
#include "CmD3D11Device.h"
#include "CmD3D11Mappings.h"
#include "CmRenderSystem.h"
#include "CmTexture.h"
#include "CmDataStream.h"

namespace CamelotEngine
{
	D3D11HardwarePixelBuffer::D3D11HardwarePixelBuffer(D3D11Texture* parentTexture, D3D11Device& device, UINT32 subresourceIndex,
		UINT32 width, UINT32 height, UINT32 depth, UINT32 face, PixelFormat format, HardwareBuffer::Usage usage):
	HardwarePixelBuffer(width, height, depth, format, usage, false),
		mParentTexture(parentTexture),
		mDevice(device),
		mSubresourceIndex(subresourceIndex),
		mFace(face),
		mDataForStaticUsageLock(0),
		mStagingBuffer(NULL)
	{
		if(mUsage & TU_RENDERTARGET)
		{
			// Create render target for each slice
			mSliceTRT.reserve(mDepth);
			for(UINT32 zoffset = 0; zoffset < mDepth; ++zoffset)
			{
				String name;
				name = "rtt/" + toString((UINT32)mParentTexture) + "/" + toString(mSubresourceIndex);

				RenderTexture *trt = new D3D11RenderTexture(name, this, mDevice);
				mSliceTRT.push_back(trt);
				
				RenderSystem::instance().attachRenderTarget(*trt);
			}
		}
	}

	D3D11HardwarePixelBuffer::~D3D11HardwarePixelBuffer()
	{
		if(!mSliceTRT.empty())
		{	
			// Delete all render targets that are not yet deleted via _clearSliceRTT
			for(UINT32 zoffset = 0; zoffset < mDepth; ++zoffset)
			{
				if(mSliceTRT[zoffset])
					RenderSystem::instance().destroyRenderTarget(mSliceTRT[zoffset]);
			}
		}

		SAFE_DELETE_ARRAY(mDataForStaticUsageLock);
		SAFE_RELEASE(mStagingBuffer);
	}
  
	// Util functions to convert a D3D locked box to a pixel box
	void D3D11HardwarePixelBuffer::fromD3DLock(PixelData& rval, const DXGI_MAPPED_RECT& lrect)
	{
		rval.rowPitch = lrect.Pitch / PixelUtil::getNumElemBytes(rval.format);
		rval.slicePitch = rval.rowPitch * rval.getHeight();
		assert((lrect.Pitch % PixelUtil::getNumElemBytes(rval.format))==0);
		rval.data = lrect.pBits;
	}

	void* D3D11HardwarePixelBuffer::_map(ID3D11Resource *res, D3D11_MAP flags)
	{
		D3D11_MAPPED_SUBRESOURCE pMappedResource;
		pMappedResource.pData = NULL;

		switch(mParentTexture->getTextureType()) 
		{
		case TEX_TYPE_1D:
			{  
				mDevice.getImmediateContext()->Map(res, static_cast<UINT>(mSubresourceIndex), flags, 0, &pMappedResource);
				if (mDevice.hasError())
				{
					String errorDescription = mDevice.getErrorDescription();
					CM_EXCEPT(RenderingAPIException, "D3D11 device cannot map 1D texture\nError Description:" + errorDescription);
				}
			}
			break;
		case TEX_TYPE_CUBE_MAP:
		case TEX_TYPE_2D:
			{
				mDevice.getImmediateContext()->Map(res, D3D11CalcSubresource(static_cast<UINT>(mSubresourceIndex), mFace, mParentTexture->getNumMipmaps()+1), 
					flags, 0, &pMappedResource);
				if (mDevice.hasError())
				{
					String errorDescription = mDevice.getErrorDescription();
					CM_EXCEPT(RenderingAPIException, "D3D11 device cannot map 2D texture\nError Description:" + errorDescription);
				}
			}
			break;
		case TEX_TYPE_2D_ARRAY:
			{
				mDevice.getImmediateContext()->Map(res, D3D11CalcSubresource(static_cast<UINT>(mSubresourceIndex), mLockBox.front, mParentTexture->getNumMipmaps()+1), 
					flags, 0, &pMappedResource);
				if (mDevice.hasError())
				{
					String errorDescription = mDevice.getErrorDescription();
					CM_EXCEPT(RenderingAPIException, "D3D11 device cannot map 2D texture array\nError Description:" + errorDescription);
				}
			}
			break;
		case TEX_TYPE_3D:
			{
				mDevice.getImmediateContext()->Map(res, static_cast<UINT>(mSubresourceIndex), flags, 0, &pMappedResource);

				if (mDevice.hasError())
				{
					String errorDescription = mDevice.getErrorDescription();
					CM_EXCEPT(RenderingAPIException, "D3D11 device cannot map 3D texture\nError Description:" + errorDescription);
				}
			}
			break;
		}

		return pMappedResource.pData;
	}

	void *D3D11HardwarePixelBuffer::_mapstaticbuffer(PixelData lock)
	{
		// for static usage just alloc
		size_t sizeOfImage = lock.getConsecutiveSize();
		
		mDataForStaticUsageLock = new UINT8[sizeOfImage];
		return mDataForStaticUsageLock;
	}
  
	void *D3D11HardwarePixelBuffer::_mapstagingbuffer(D3D11_MAP flags)
	{
		if(!mStagingBuffer)
			createStagingBuffer();

		if(flags == D3D11_MAP_READ_WRITE || flags == D3D11_MAP_READ || flags == D3D11_MAP_WRITE)  
		{
			if(mLockBox.getHeight() == mParentTexture->getHeight() && mLockBox.getWidth() == mParentTexture->getWidth())
				mDevice.getImmediateContext()->CopyResource(mStagingBuffer, mParentTexture->getTextureResource());
			else
			{
				D3D11_BOX dstBoxDx11 = convertBoxToDx11Box(mLockBox);
				dstBoxDx11.front = 0;
				dstBoxDx11.back = mLockBox.getDepth();

				unsigned int subresource = D3D11CalcSubresource(mSubresourceIndex, mLockBox.front, mParentTexture->getNumMipmaps()+1);
				mDevice.getImmediateContext()->CopySubresourceRegion(mStagingBuffer, subresource, mLockBox.left, mLockBox.top, 
					mSubresourceIndex, mParentTexture->getTextureResource(), subresource, &dstBoxDx11);
			}
		}
		else if(flags == D3D11_MAP_WRITE_DISCARD)
			flags = D3D11_MAP_WRITE; // stagingbuffer doesn't support discarding

		return _map(mStagingBuffer, flags);
	}
 
	PixelData D3D11HardwarePixelBuffer::lockImpl(const Box lockBox, LockOptions options)
	{
		// Check for misuse
		if(mUsage & TU_RENDERTARGET)
			CM_EXCEPT(RenderingAPIException, "DirectX does not allow locking of or directly writing to RenderTargets. Use blitFromMemory if you need the contents.");	

		mLockBox = lockBox;

		// Set extents and format
		// Note that we do not carry over the left/top/front here, since the returned
		// PixelBox will be re-based from the locking point onwards
		PixelData rval(lockBox.getWidth(), lockBox.getHeight(), lockBox.getDepth(), mFormat);
		// Set locking flags according to options
		D3D11_MAP  flags = D3D11_MAP_WRITE_DISCARD ;
		switch(options)
		{
		case HBL_WRITE_ONLY_NO_OVERWRITE:
			flags = D3D11_MAP_WRITE_NO_OVERWRITE;
			break;
		case HBL_READ_WRITE:
			flags = D3D11_MAP_READ_WRITE;
			break;
		case HBL_WRITE_ONLY_DISCARD:
			flags = D3D11_MAP_WRITE_DISCARD;
			break;
		case HBL_READ_ONLY:
			flags = D3D11_MAP_READ;
			break;
		case HBL_WRITE_ONLY:
			flags = D3D11_MAP_WRITE;
			break;
		default: 
			break;
		};

		size_t offset = 0;

		if(mUsage == HBU_STATIC || mUsage & HBU_DYNAMIC)
		{
			if(mUsage == HBU_STATIC || options == HBL_READ_ONLY || options == HBL_READ_WRITE || options == HBL_WRITE_ONLY)
				rval.data = _mapstagingbuffer(flags);
			else
				rval.data = _map(mParentTexture->getTextureResource(), flags);

			// calculate the offset in bytes
			offset = D3D11Mappings::_getSizeInBytes(rval.format, rval.left, rval.front);
			// add the offset, so the right memory will be changed
			//rval.data = static_cast<int*>(rval.data) + offset;
		}
		else
		{
			size_t sizeOfImage = rval.getConsecutiveSize();
            mDataForStaticUsageLock = new UINT8[sizeOfImage];
            rval.data = mDataForStaticUsageLock;
		}
		// save without offset
 		mCurrentLock = rval;
		mCurrentLockOptions = options;

		// add the offset, so the right memory will be changed
		rval.data = static_cast<int*>(rval.data) + offset;

		return rval;
	}

	void D3D11HardwarePixelBuffer::_unmap(ID3D11Resource *res)
	{
		switch(mParentTexture->getTextureType()) {
		case TEX_TYPE_1D:
			{
				mDevice.getImmediateContext()->Unmap(res, mSubresourceIndex);
			}
			break;
		case TEX_TYPE_CUBE_MAP:
		case TEX_TYPE_2D:
			{							  
				mDevice.getImmediateContext()->Unmap(res, D3D11CalcSubresource(static_cast<UINT>(mSubresourceIndex), mFace, mParentTexture->getNumMipmaps()+1));
			}
			break;
		case TEX_TYPE_2D_ARRAY:
			{
				mDevice.getImmediateContext()->Unmap(res, D3D11CalcSubresource(static_cast<UINT>(mSubresourceIndex), mLockBox.front, mParentTexture->getNumMipmaps()+1));
			}
			break;
		case TEX_TYPE_3D:
			{
				mDevice.getImmediateContext()->Unmap(res, mSubresourceIndex);
			}
			break;
		}

		if (mDevice.hasError())
		{
			String errorDescription = mDevice.getErrorDescription();
			CM_EXCEPT(RenderingAPIException, "D3D11 device unmap resource\nError Description:" + errorDescription);
		}
	}

	void D3D11HardwarePixelBuffer::_unmapstaticbuffer()
	{
		D3D11_BOX dstBoxDx11 = convertBoxToDx11Box(mLockBox);
		dstBoxDx11.front = 0;
		dstBoxDx11.back = mLockBox.getDepth();

		size_t rowWidth = D3D11Mappings::_getSizeInBytes(mCurrentLock.format, mCurrentLock.getWidth());

		switch(mParentTexture->getTextureType()) {
		case TEX_TYPE_1D:
			{

				mDevice.getImmediateContext()->UpdateSubresource(mParentTexture->GetTex1D(), 
					static_cast<UINT>(mSubresourceIndex), &dstBoxDx11, 
					mDataForStaticUsageLock, rowWidth, 0);
				if (mDevice.hasError())
				{
					String errorDescription = mDevice.getErrorDescription();
					CM_EXCEPT(RenderingAPIException, "D3D11 device cannot map 1D texture\nError Description:" + errorDescription);
				}
			}
			break;
		case TEX_TYPE_CUBE_MAP:
		case TEX_TYPE_2D:
			{
				mDevice.getImmediateContext()->UpdateSubresource(mParentTexture->GetTex2D(), 
					D3D11CalcSubresource(static_cast<UINT>(mSubresourceIndex), mFace, mParentTexture->getNumMipmaps()+1),
					&dstBoxDx11, 
					mDataForStaticUsageLock, rowWidth, 0);

				if (mDevice.hasError())
				{
					String errorDescription = mDevice.getErrorDescription();
					CM_EXCEPT(RenderingAPIException, "D3D11 device cannot map 2D texture\nError Description:" + errorDescription);
				}
			}
			break;
		case TEX_TYPE_2D_ARRAY:
			{
				mDevice.getImmediateContext()->UpdateSubresource(mParentTexture->GetTex2D(), 
					D3D11CalcSubresource(static_cast<UINT>(mSubresourceIndex), mLockBox.front, mParentTexture->getNumMipmaps()+1),
					&dstBoxDx11, mDataForStaticUsageLock, rowWidth, 0);

				if (mDevice.hasError())
				{
					String errorDescription = mDevice.getErrorDescription();
					CM_EXCEPT(RenderingAPIException, "D3D11 device cannot map 2D texture array\nError Description:" + errorDescription);
				}
			}
			break;
		case TEX_TYPE_3D:
			{
				size_t sliceWidth = D3D11Mappings::_getSizeInBytes(mCurrentLock.format, mCurrentLock.getWidth(), mCurrentLock.getHeight());

				mDevice.getImmediateContext()->UpdateSubresource(mParentTexture->GetTex3D(), static_cast<UINT>(mSubresourceIndex), 
					&dstBoxDx11, mDataForStaticUsageLock, rowWidth, sliceWidth);
				if (mDevice.hasError())
				{
					String errorDescription = mDevice.getErrorDescription();
					CM_EXCEPT(RenderingAPIException, "D3D11 device cannot map 3D texture\nError Description:" + errorDescription);
				}
			}
			break;
		}

		SAFE_DELETE_ARRAY(mDataForStaticUsageLock) ;
	}
  
	void D3D11HardwarePixelBuffer::_unmapstagingbuffer(bool copyback)
	{
		_unmap(mStagingBuffer);

		if(copyback)
		{
			if(mLockBox.getHeight() == mParentTexture->getHeight() && mLockBox.getWidth() == mParentTexture->getWidth())
				mDevice.getImmediateContext()->CopyResource(mParentTexture->getTextureResource(), mStagingBuffer);
			else
			{
				D3D11_BOX dstBoxDx11 = convertBoxToDx11Box(mLockBox);
				dstBoxDx11.front = 0;
				dstBoxDx11.back = mLockBox.getDepth();

				unsigned int subresource = D3D11CalcSubresource(mSubresourceIndex, mLockBox.front, mParentTexture->getNumMipmaps()+1);
				mDevice.getImmediateContext()->CopySubresourceRegion(mParentTexture->getTextureResource(), subresource, mLockBox.left, mLockBox.top, mSubresourceIndex, mStagingBuffer, subresource, &dstBoxDx11);
			}
		}
	}
 
	void D3D11HardwarePixelBuffer::unlockImpl(void)
	{
		if(mUsage == HBU_STATIC)
			_unmapstagingbuffer();
		else if(mUsage & HBU_DYNAMIC)
 		{
			if(mCurrentLockOptions == HBL_READ_ONLY || mCurrentLockOptions == HBL_READ_WRITE || mCurrentLockOptions == HBL_WRITE_ONLY)
			{
				size_t sizeinbytes = D3D11Mappings::_getSizeInBytes(mParentTexture->getFormat(), mParentTexture->getWidth(), mParentTexture->getHeight());

				void *data = _map(mParentTexture->getTextureResource(), D3D11_MAP_WRITE_DISCARD);

				memcpy(data, mCurrentLock.data, sizeinbytes);

				// unmap the texture and the staging buffer
				_unmap(mParentTexture->getTextureResource());

				_unmapstagingbuffer(false);
 			}
			else
				_unmap(mParentTexture->getTextureResource());
 		}
 		else
			_unmapstaticbuffer();
	}

	D3D11_BOX D3D11HardwarePixelBuffer::convertBoxToDx11Box(const Box &inBox) const
	{
		D3D11_BOX res;
		res.left	= static_cast<UINT>(inBox.left);
		res.top		= static_cast<UINT>(inBox.top);
		res.front	= static_cast<UINT>(inBox.front);
		res.right	= static_cast<UINT>(inBox.right);
		res.bottom	= static_cast<UINT>(inBox.bottom);
		res.back	= static_cast<UINT>(inBox.back);

		return res;
	}

	void D3D11HardwarePixelBuffer::blit(const HardwarePixelBufferPtr &rsrc, const Box &srcBox, const Box &dstBox)
	{
		if (
			(srcBox.getWidth() != dstBox.getWidth())
			|| (srcBox.getHeight() != dstBox.getHeight())
			|| (srcBox.getDepth() != dstBox.getDepth())
			)
		{
			CM_EXCEPT(RenderingAPIException, 
				"D3D11 device cannot copy a subresource - source and dest size are not the same and they have to be the same in DX11.");
		}

		D3D11_BOX srcBoxDx11 = convertBoxToDx11Box(srcBox);
		D3D11HardwarePixelBuffer* rsrcDx11 = static_cast<D3D11HardwarePixelBuffer*>(rsrc.get());

		switch(mParentTexture->getTextureType()) {
		case TEX_TYPE_1D:
			{

				mDevice.getImmediateContext()->CopySubresourceRegion(
					mParentTexture->GetTex1D(), 
					static_cast<UINT>(mSubresourceIndex),
					static_cast<UINT>(dstBox.left),
					0,
					0,
					rsrcDx11->mParentTexture->GetTex1D(),
					static_cast<UINT>(rsrcDx11->mSubresourceIndex),
					&srcBoxDx11);
				if (mDevice.hasError())
				{
					String errorDescription = mDevice.getErrorDescription();
					CM_EXCEPT(RenderingAPIException, "D3D11 device cannot copy 1d subresource Region\nError Description:" + errorDescription);
				}			
			}
			break;
		case TEX_TYPE_CUBE_MAP:
		case TEX_TYPE_2D:
			{
				mDevice.getImmediateContext()->CopySubresourceRegion(
					mParentTexture->GetTex2D(), 
					D3D11CalcSubresource(static_cast<UINT>(mSubresourceIndex), mFace, mParentTexture->getNumMipmaps()+1),
					static_cast<UINT>(dstBox.left),
					static_cast<UINT>(dstBox.top),
					mFace,
					rsrcDx11->mParentTexture->GetTex2D(),
					static_cast<UINT>(rsrcDx11->mSubresourceIndex),
					&srcBoxDx11);
				if (mDevice.hasError())
				{
					String errorDescription = mDevice.getErrorDescription();
					CM_EXCEPT(RenderingAPIException, "D3D11 device cannot copy 2d subresource Region\nError Description:" + errorDescription);
				}
			}
			break;
		case TEX_TYPE_2D_ARRAY:
			{
				mDevice.getImmediateContext()->CopySubresourceRegion(
					mParentTexture->GetTex2D(), 
					D3D11CalcSubresource(static_cast<UINT>(mSubresourceIndex), srcBox.front, mParentTexture->getNumMipmaps()+1),
					static_cast<UINT>(dstBox.left),
					static_cast<UINT>(dstBox.top),
					srcBox.front,
					rsrcDx11->mParentTexture->GetTex2D(),
					static_cast<UINT>(rsrcDx11->mSubresourceIndex),
					&srcBoxDx11);
				if (mDevice.hasError())
				{
					String errorDescription = mDevice.getErrorDescription();
					CM_EXCEPT(RenderingAPIException, "D3D11 device cannot copy 2d subresource Region\nError Description:" + errorDescription);
				}
			}
			break;
		case TEX_TYPE_3D:
			{
				mDevice.getImmediateContext()->CopySubresourceRegion(
					mParentTexture->GetTex3D(), 
					static_cast<UINT>(mSubresourceIndex),
					static_cast<UINT>(dstBox.left),
					static_cast<UINT>(dstBox.top),
					static_cast<UINT>(dstBox.front),
					rsrcDx11->mParentTexture->GetTex3D(),
					static_cast<UINT>(rsrcDx11->mSubresourceIndex),
					&srcBoxDx11);
				if (mDevice.hasError())
				{
					String errorDescription = mDevice.getErrorDescription();
					CM_EXCEPT(RenderingAPIException, "D3D11 device cannot copy 3d subresource Region\nError Description:" + errorDescription);
				}
			}
			break;
		}

		_genMipmaps();
	}
 
	void D3D11HardwarePixelBuffer::blitFromMemory(const PixelData& src, const Box& dstBox)
	{
		bool isCompressed = false;
		switch(mFormat)
		{
		case PF_DXT1:
		case PF_DXT2:
		case PF_DXT3:
		case PF_DXT4:
		case PF_DXT5:
			isCompressed = true;
			break;
		default:

			break;
		}

		if (isCompressed && (dstBox.getWidth() % 4 != 0 || dstBox.getHeight() % 4 != 0 ))
		{
			return;
		}


		// for scoped deletion of conversion buffer
		PixelData converted = src;

		D3D11_BOX dstBoxDx11 = convertBoxToDx11Box(dstBox);
		dstBoxDx11.front = 0;
		dstBoxDx11.back = converted.getDepth();

		// convert to pixelbuffer's native format if necessary
		if (src.format != mFormat)
		{
			MemoryDataStreamPtr buf(new MemoryDataStream(PixelUtil::getMemorySize(src.getWidth(), src.getHeight(), src.getDepth(), mFormat)));
			converted = PixelData(src.getWidth(), src.getHeight(), src.getDepth(), mFormat, buf->getPtr());
			PixelUtil::bulkPixelConversion(src, converted);
		}

		if (mUsage & HBU_DYNAMIC)
		{
			size_t sizeinbytes;
			if (PixelUtil::isCompressed(converted.format))
 			{
				// D3D wants the width of one row of cells in bytes
				if (converted.format == PF_DXT1)
				{
					// 64 bits (8 bytes) per 4x4 block
					sizeinbytes = std::max<size_t>(1, converted.getWidth() / 4) * std::max<size_t>(1, converted.getHeight() / 4) * 8;
				}
				else
				{
					// 128 bits (16 bytes) per 4x4 block
					sizeinbytes = std::max<size_t>(1, converted.getWidth() / 4) * std::max<size_t>(1, converted.getHeight() / 4) * 16;
				}
 			}
 			else
 			{
				sizeinbytes = converted.getHeight() * converted.getWidth() * PixelUtil::getNumElemBytes(converted.format);
 			}
 
			const PixelData& locked = lock(dstBox, HBL_WRITE_ONLY_DISCARD);

			memcpy(locked.data, converted.data, sizeinbytes);

			unlock();
 		}
 		else
		{
			size_t rowWidth;
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

			switch(mParentTexture->getTextureType()) {
			case TEX_TYPE_1D:
 				{

					mDevice.getImmediateContext()->UpdateSubresource( 
						mParentTexture->GetTex1D(), 
						0,
						&dstBoxDx11,
						converted.data,
						rowWidth,
						0 );
					if (mDevice.hasError())
					{
						String errorDescription = mDevice.getErrorDescription();
						CM_EXCEPT(RenderingAPIException, "D3D11 device cannot update 1d subresource\nError Description:" + errorDescription);
					}
 				}
				break;
			case TEX_TYPE_CUBE_MAP:
			case TEX_TYPE_2D:
 				{
					mDevice.getImmediateContext()->UpdateSubresource( 
						mParentTexture->GetTex2D(), 
						D3D11CalcSubresource(static_cast<UINT>(mSubresourceIndex), mFace, mParentTexture->getNumMipmaps()+1),
						&dstBoxDx11,
						converted.data,
						rowWidth,
						0 );
					if (mDevice.hasError())
					{
						String errorDescription = mDevice.getErrorDescription();
						CM_EXCEPT(RenderingAPIException, "D3D11 device cannot update 2d subresource\nError Description:" + errorDescription);
					}
 				}
				break;
			case TEX_TYPE_2D_ARRAY:
 				{
					mDevice.getImmediateContext()->UpdateSubresource( 
						mParentTexture->GetTex2D(), 
						D3D11CalcSubresource(static_cast<UINT>(mSubresourceIndex), src.front, mParentTexture->getNumMipmaps()+1),
						&dstBoxDx11,
						converted.data,
						rowWidth,
						0 );
					if (mDevice.hasError())
 					{
						String errorDescription = mDevice.getErrorDescription();
						CM_EXCEPT(RenderingAPIException, "D3D11 device cannot update 2d array subresource\nError Description:" + errorDescription);
				}
				}
				break;
			case TEX_TYPE_3D:
				{
					// copied from dx9
					size_t sliceWidth;
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
 
					mDevice.getImmediateContext()->UpdateSubresource( 
						mParentTexture->GetTex3D(), 
						static_cast<UINT>(mSubresourceIndex),
						&dstBoxDx11,
						converted.data,
						rowWidth,
						sliceWidth
						);
					if (mDevice.hasError())
					{
						String errorDescription = mDevice.getErrorDescription();
						CM_EXCEPT(RenderingAPIException, "D3D11 device cannot update 3d subresource\nError Description:" + errorDescription);
					}
 				}
				break;
			}
 
			if (!isCompressed)
			{
				_genMipmaps();
 			}
		}	
 	}

	void D3D11HardwarePixelBuffer::blitToMemory(const Box &srcBox, const PixelData &dst)
	{
		if(mParentTexture->getTextureType() != TEX_TYPE_2D)
		{
			CM_EXCEPT(NotImplementedException, "Only implemeted for 2D buffers.");
		}

		PixelData lockedData = lock(srcBox, HardwareBuffer::HBL_READ_ONLY);

		if(dst.getConsecutiveSize() != lockedData.getConsecutiveSize())
		{
			unlock();

			CM_EXCEPT(RenderingAPIException, "Size of the provided buffer (" + toString(dst.getConsecutiveSize()) + "), doesn't "
				+ "match the size of the texture buffer (" + toString(lockedData.getConsecutiveSize()) + ").");
		}

		memcpy(dst.data, lockedData.data, lockedData.getConsecutiveSize());

		unlock();
	}
  
	void D3D11HardwarePixelBuffer::_genMipmaps()
	{
        if(mParentTexture->HasAutoMipMapGenerationEnabled())
 		{
            ID3D11ShaderResourceView *pShaderResourceView = mParentTexture->getTexture();
			mDevice.getImmediateContext()->GenerateMips(pShaderResourceView);
			if (mDevice.hasError())
			{
				String errorDescription = mDevice.getErrorDescription();
				CM_EXCEPT(RenderingAPIException, "D3D11 device cannot generate mips\nError Description:" + errorDescription);
			}	
		}
	}
    
	RenderTexture *D3D11HardwarePixelBuffer::getRenderTarget(UINT32 zoffset)
	{
		assert(mUsage & TU_RENDERTARGET);
		assert(zoffset < mDepth);
		return mSliceTRT[zoffset];
	}

	D3D11Texture * D3D11HardwarePixelBuffer::getParentTexture() const
	{
		return mParentTexture;
	}
    
	UINT32 D3D11HardwarePixelBuffer::getSubresourceIndex() const
	{
		return mSubresourceIndex;
	}
   
	UINT32 D3D11HardwarePixelBuffer::getFace() const
	{
		return mFace;
	}
   
	void D3D11HardwarePixelBuffer::createStagingBuffer()
	{
		switch (mParentTexture->getTextureType())
		{
		case TEX_TYPE_1D:
			{
				D3D11_TEXTURE1D_DESC desc;
				mParentTexture->GetTex1D()->GetDesc(&desc);

				desc.BindFlags = 0;
				desc.MiscFlags = 0;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
				desc.Usage = D3D11_USAGE_STAGING;

				mDevice.getD3D11Device()->CreateTexture1D(&desc, NULL, (ID3D11Texture1D**)(&mStagingBuffer));
			} 					
			break;
		case TEX_TYPE_2D:
		case TEX_TYPE_CUBE_MAP:
		case TEX_TYPE_2D_ARRAY:
			{
				D3D11_TEXTURE2D_DESC desc;
				mParentTexture->GetTex2D()->GetDesc(&desc);

				desc.BindFlags = 0;
				desc.MiscFlags = 0;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
				desc.Usage = D3D11_USAGE_STAGING;

				mDevice.getD3D11Device()->CreateTexture2D(&desc, NULL, (ID3D11Texture2D**)(&mStagingBuffer));
			}
			break;
		case TEX_TYPE_3D:
			{
				D3D11_TEXTURE3D_DESC desc;
				mParentTexture->GetTex3D()->GetDesc(&desc);

				desc.BindFlags = 0;
				desc.MiscFlags = 0;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
				desc.Usage = D3D11_USAGE_STAGING;

				mDevice.getD3D11Device()->CreateTexture3D(&desc, NULL, (ID3D11Texture3D**)(&mStagingBuffer));
			}
			break;
		}
	}
}
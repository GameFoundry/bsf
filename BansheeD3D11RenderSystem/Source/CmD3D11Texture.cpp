#include "CmD3D11Texture.h"
#include "CmD3D11Mappings.h"
#include "CmD3D11Device.h"
#include "CmD3D11RenderSystem.h"
#include "CmD3D11TextureView.h"
#include "CmCoreThread.h"
#include "CmException.h"
#include "CmAsyncOp.h"
#include "CmDebug.h"

namespace BansheeEngine
{
	D3D11Texture::D3D11Texture()
		: Texture()
		, m1DTex(nullptr)
		, m2DTex(nullptr)
		, m3DTex(nullptr)
		, mTex(nullptr)
		, mShaderResourceView(nullptr)
		, mStagingBuffer(nullptr)
		, mLockedSubresourceIdx(-1)
		, mLockedForReading(false)
		, mStaticBuffer(nullptr)
	{

	}

	D3D11Texture::~D3D11Texture()
	{
			
	}

	void D3D11Texture::copyImpl(TexturePtr& target)
	{
		D3D11Texture* other = static_cast<D3D11Texture*>(target.get());

		D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		D3D11Device& device = rs->getPrimaryDevice();
		device.getImmediateContext()->CopyResource(other->getDX11Resource(), mTex);

		if (device.hasError())
		{
			String errorDescription = device.getErrorDescription();
			BS_EXCEPT(RenderingAPIException, "D3D11 device cannot copy resource\nError Description:" + errorDescription);
		}
	}

	PixelData D3D11Texture::lockImpl(GpuLockOptions options, UINT32 mipLevel, UINT32 face)
	{
		UINT32 mipWidth = mWidth >> mipLevel;
		UINT32 mipHeight = mHeight >> mipLevel;
		UINT32 mipDepth = mDepth >> mipLevel;

		PixelData lockedArea(mipWidth, mipHeight, mipDepth, mFormat);

		D3D11_MAP flags = D3D11Mappings::getLockOptions(options);

		UINT32 rowPitch, slicePitch;

		if(flags == D3D11_MAP_READ || flags == D3D11_MAP_READ_WRITE)
		{
			UINT8* data = (UINT8*)mapstagingbuffer(flags, face, mipLevel, rowPitch, slicePitch);
			lockedArea.setExternalBuffer(data);
			lockedArea.setRowPitch(rowPitch);
			lockedArea.setSlicePitch(slicePitch);
			mLockedForReading = true;
		}
		else
		{
			if(mUsage == TU_DYNAMIC)
			{
				UINT8* data = (UINT8*)map(mTex, flags, face, mipLevel, rowPitch, slicePitch);
				lockedArea.setExternalBuffer(data);
				lockedArea.setRowPitch(rowPitch);
				lockedArea.setSlicePitch(slicePitch);
			}
			else
				lockedArea.setExternalBuffer((UINT8*)mapstaticbuffer(lockedArea, mipLevel, face));

			mLockedForReading = false;
		}

		return lockedArea;
	}

	void D3D11Texture::unlockImpl()
	{
		if(mLockedForReading)
			unmapstagingbuffer();
		else
		{
			if(mUsage == TU_DYNAMIC)
				unmap(mTex);
			else
				unmapstaticbuffer();
		}
	}

	void D3D11Texture::readData(PixelData& dest, UINT32 mipLevel, UINT32 face)
	{
		PixelData myData = lock(GBL_READ_ONLY, mipLevel, face);

#if BS_DEBUG_MODE
		if(dest.getConsecutiveSize() != myData.getConsecutiveSize())
		{
			unlock();
			BS_EXCEPT(InternalErrorException, "Buffer sizes don't match");
		}
#endif

		PixelUtil::bulkPixelConversion(myData, dest);

		unlock();
	}

	void D3D11Texture::writeData(const PixelData& src, UINT32 mipLevel, UINT32 face, bool discardWholeBuffer)
	{
		if(mUsage == TU_DYNAMIC)
		{
			PixelData myData = lock(discardWholeBuffer ? GBL_WRITE_ONLY_DISCARD : GBL_WRITE_ONLY, mipLevel, face);
			PixelUtil::bulkPixelConversion(src, myData);
			unlock();
		}
		else if(mUsage == TU_STATIC)
		{
			mipLevel = Math::clamp(mipLevel, (UINT32)mipLevel, getNumMipmaps());
			face = Math::clamp(face, (UINT32)0, mDepth - 1);

			if(getTextureType() == TEX_TYPE_3D)
				face = 0;

			D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
			D3D11Device& device = rs->getPrimaryDevice();

			UINT subresourceIdx = D3D11CalcSubresource(mipLevel, face, getNumMipmaps()+1);
			UINT32 rowWidth = D3D11Mappings::_getSizeInBytes(mFormat, src.getWidth());
			UINT32 sliceWidth = D3D11Mappings::_getSizeInBytes(mFormat, src.getWidth(), src.getHeight());

			device.getImmediateContext()->UpdateSubresource(mTex, subresourceIdx, nullptr, src.getData(), rowWidth, sliceWidth);

			if (device.hasError())
			{
				String errorDescription = device.getErrorDescription();
				BS_EXCEPT(RenderingAPIException, "D3D11 device cannot map texture\nError Description:" + errorDescription);
			}
		}
		else
		{
			BS_EXCEPT(RenderingAPIException, "Trying to write into a buffer with unsupported usage: " + toString(mUsage));
		}
	}

	void D3D11Texture::initialize_internal()
	{
		THROW_IF_NOT_CORE_THREAD;

		// load based on tex.type
		switch (getTextureType())
		{
			case TEX_TYPE_1D:
				create1DTex();
				break;
			case TEX_TYPE_2D:
			case TEX_TYPE_CUBE_MAP:
				create2DTex();
				break;
			case TEX_TYPE_3D:
				create3DTex();
				break;
			default:
				destroy_internal();
				BS_EXCEPT(RenderingAPIException, "Unknown texture type");
		}

		Texture::initialize_internal();
	}

	void D3D11Texture::destroy_internal()
	{
		SAFE_RELEASE(mTex);
		SAFE_RELEASE(mShaderResourceView);
		SAFE_RELEASE(m1DTex);
		SAFE_RELEASE(m2DTex);
		SAFE_RELEASE(m3DTex);
		SAFE_RELEASE(mStagingBuffer);

		clearBufferViews();

		Texture::destroy_internal();
	}

	void D3D11Texture::create1DTex()
	{
		// We must have those defined here
		assert(mWidth > 0);

		// Determine which D3D11 pixel format we'll use
		HRESULT hr;
		DXGI_FORMAT d3dPF = D3D11Mappings::_getPF(D3D11Mappings::_getClosestSupportedPF(mFormat));

		if(mFormat != D3D11Mappings::_getPF(d3dPF))
		{
			BS_EXCEPT(RenderingAPIException, "Provided pixel format is not supported by the driver: " + toString(mFormat));
		}

		D3D11_TEXTURE1D_DESC desc;
		desc.Width			= static_cast<UINT32>(mWidth);
		desc.ArraySize		= 1;
		desc.Format			= d3dPF;
		desc.MiscFlags		= 0;

		if((mUsage & TU_RENDERTARGET) != 0)
		{
			desc.Usage			= D3D11_USAGE_DEFAULT;
			desc.BindFlags		= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.MipLevels		= 1;
		}
		else if((mUsage & TU_DEPTHSTENCIL) != 0)
		{
			desc.Usage			= D3D11_USAGE_DEFAULT;
			desc.BindFlags		= D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.MipLevels		= 1;
		}
		else
		{
			desc.Usage			= D3D11Mappings::_getUsage(mUsage);
			desc.BindFlags		= D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = D3D11Mappings::_getAccessFlags(mUsage);

			// Determine total number of mipmaps including main one (d3d11 convention)
			UINT32 numMips		= (mNumMipmaps == MIP_UNLIMITED || (1U << mNumMipmaps) > mWidth) ? 0 : mNumMipmaps + 1;
			desc.MipLevels		= numMips;
		}

		// Create the texture
		D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		D3D11Device& device = rs->getPrimaryDevice();
		hr = device.getD3D11Device()->CreateTexture1D(&desc, nullptr, &m1DTex);

		// Check result and except if failed
		if (FAILED(hr) || device.hasError())
		{
			destroy_internal();
			String errorDescription = device.getErrorDescription();
			BS_EXCEPT(RenderingAPIException, "Error creating texture\nError Description:" + errorDescription);
		}

		hr = m1DTex->QueryInterface(__uuidof(ID3D11Resource), (void **)&mTex);

		if(FAILED(hr) || device.hasError())
		{
			destroy_internal();
			String errorDescription = device.getErrorDescription();
			BS_EXCEPT(RenderingAPIException, "Can't get base texture\nError Description:" + errorDescription);
		}

		m1DTex->GetDesc(&desc);

		if(mNumMipmaps != (desc.MipLevels - 1))
		{
			BS_EXCEPT(RenderingAPIException, "Driver returned different number of mip maps than requested. " \
				"Requested: " + toString(mNumMipmaps) + ". Got: " + toString(desc.MipLevels - 1) + ".");
		}

		mDimension = D3D11_SRV_DIMENSION_TEXTURE1D;

		// Create texture view
		if((mUsage & TU_DEPTHSTENCIL) == 0)
		{
			ZeroMemory(&mSRVDesc, sizeof(mSRVDesc));
			mSRVDesc.Format = desc.Format;
			mSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D; 
			mSRVDesc.Texture1D.MipLevels = desc.MipLevels;
			hr = device.getD3D11Device()->CreateShaderResourceView(m1DTex, &mSRVDesc, &mShaderResourceView);

			if (FAILED(hr) || device.hasError())
			{
				String errorDescription = device.getErrorDescription();
				BS_EXCEPT(RenderingAPIException, "D3D11 device can't create shader resource view.\nError Description:" + errorDescription);
			}
		}
	}

	void D3D11Texture::create2DTex()
	{
		// We must have those defined here
		assert(mWidth > 0 || mHeight > 0);

		// Determine which D3D11 pixel format we'll use
		HRESULT hr;
		DXGI_FORMAT d3dPF = D3D11Mappings::_getPF(D3D11Mappings::_getClosestSupportedPF(mFormat));

		if(mFormat != D3D11Mappings::_getPF(d3dPF))
		{
			BS_EXCEPT(RenderingAPIException, "Provided pixel format is not supported by the driver: " + toString(mFormat));
		}

		D3D11_TEXTURE2D_DESC desc;
		desc.Width			= static_cast<UINT32>(mWidth);
		desc.Height			= static_cast<UINT32>(mHeight);
		desc.ArraySize		= 1;
		desc.Format			= d3dPF;
		desc.MiscFlags		= 0;

		if((mUsage & TU_RENDERTARGET) != 0)
		{
			desc.Usage			= D3D11_USAGE_DEFAULT;
			desc.BindFlags		= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.MipLevels		= 1;

			DXGI_SAMPLE_DESC sampleDesc;
			D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
			rs->determineMultisampleSettings(mMultisampleCount, mMultisampleHint, d3dPF, &sampleDesc);
			desc.SampleDesc		= sampleDesc;

			if(getTextureType() == TEX_TYPE_CUBE_MAP)
			{
				BS_EXCEPT(NotImplementedException, "Cube map not yet supported as a render target."); // TODO: Will be once I add proper texture array support
			}
		}
		else if((mUsage & TU_DEPTHSTENCIL) != 0)
		{
			desc.Usage			= D3D11_USAGE_DEFAULT;
			desc.BindFlags		= D3D11_BIND_DEPTH_STENCIL;
			desc.CPUAccessFlags = 0;
			desc.MipLevels		= 1;

			DXGI_SAMPLE_DESC sampleDesc;
			D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
			rs->determineMultisampleSettings(mMultisampleCount, mMultisampleHint, d3dPF, &sampleDesc);
			desc.SampleDesc		= sampleDesc;

			if(getTextureType() == TEX_TYPE_CUBE_MAP)
			{
				BS_EXCEPT(NotImplementedException, "Cube map not yet supported as a depth stencil target."); // TODO: Will be once I add proper texture array support
			}
		}
		else
		{
			desc.Usage			= D3D11Mappings::_getUsage(mUsage);
			desc.BindFlags		= D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = D3D11Mappings::_getAccessFlags(mUsage);

			// Determine total number of mipmaps including main one (d3d11 convention)
			UINT32 numMips = (mNumMipmaps == MIP_UNLIMITED || (1U << mNumMipmaps) > mWidth) ? 0 : mNumMipmaps + 1;
			desc.MipLevels		= numMips;

			DXGI_SAMPLE_DESC sampleDesc;
			sampleDesc.Count	= 1;
			sampleDesc.Quality	= 0;
			desc.SampleDesc		= sampleDesc;
		}

        if (getTextureType() == TEX_TYPE_CUBE_MAP)
        {
            desc.MiscFlags      |= D3D11_RESOURCE_MISC_TEXTURECUBE;
            desc.ArraySize       = 6;
        }

		// Create the texture
		D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		D3D11Device& device = rs->getPrimaryDevice();
		hr = device.getD3D11Device()->CreateTexture2D(&desc, nullptr, &m2DTex);

		// Check result and except if failed
		if (FAILED(hr) || device.hasError())
		{
			destroy_internal();
			String errorDescription = device.getErrorDescription();
			BS_EXCEPT(RenderingAPIException, "Error creating texture\nError Description:" + errorDescription);
		}

		hr = m2DTex->QueryInterface(__uuidof(ID3D11Resource), (void **)&mTex);

		if(FAILED(hr) || device.hasError())
		{
			destroy_internal();
			String errorDescription = device.getErrorDescription();
			BS_EXCEPT(RenderingAPIException, "Can't get base texture\nError Description:" + errorDescription);
		}

		m2DTex->GetDesc(&desc);

		if(mNumMipmaps != (desc.MipLevels - 1))
		{
			BS_EXCEPT(RenderingAPIException, "Driver returned different number of mip maps than requested. " \
				"Requested: " + toString(mNumMipmaps) + ". Got: " + toString(desc.MipLevels - 1) + ".");
		}

		mDXGIFormat = desc.Format;

		// Create shader texture view
		if((mUsage & TU_DEPTHSTENCIL) == 0)
		{
			ZeroMemory(&mSRVDesc, sizeof(mSRVDesc));
			mSRVDesc.Format = desc.Format;

			if((mUsage & TU_RENDERTARGET) != 0)
			{
				if(mMultisampleCount > 0)
				{
					mSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
					mSRVDesc.Texture2D.MostDetailedMip = 0;
					mSRVDesc.Texture2D.MipLevels = desc.MipLevels;
				}
				else
				{
					mSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
					mSRVDesc.Texture2D.MostDetailedMip = 0;
					mSRVDesc.Texture2D.MipLevels = desc.MipLevels;
				}
			}
			else
			{
				switch(getTextureType())
				{
				case TEX_TYPE_CUBE_MAP:
					mSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
					mSRVDesc.TextureCube.MipLevels = desc.MipLevels;
					mSRVDesc.TextureCube.MostDetailedMip = 0;
					break;

				case TEX_TYPE_2D:
					mSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
					mSRVDesc.Texture2D.MostDetailedMip = 0;
					mSRVDesc.Texture2D.MipLevels = desc.MipLevels;
					break;
				}
			}

			mDimension = mSRVDesc.ViewDimension;
			hr = device.getD3D11Device()->CreateShaderResourceView(m2DTex, &mSRVDesc, &mShaderResourceView);

			if (FAILED(hr) || device.hasError())
			{
				String errorDescription = device.getErrorDescription();
				BS_EXCEPT(RenderingAPIException, "D3D11 device can't create shader resource view.\nError Description:" + errorDescription);
			}
		}
		else
		{
			if(mMultisampleCount > 0)
				mDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
			else
				mDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		}
	}

	void D3D11Texture::create3DTex()
	{
		// We must have those defined here
		assert(mWidth > 0 && mHeight > 0 && mDepth > 0);

		// Determine which D3D11 pixel format we'll use
		HRESULT hr;
		DXGI_FORMAT d3dPF = D3D11Mappings::_getPF(D3D11Mappings::_getClosestSupportedPF(mFormat));
		
		if(mFormat != D3D11Mappings::_getPF(d3dPF))
		{
			BS_EXCEPT(RenderingAPIException, "Provided pixel format is not supported by the driver: " + toString(mFormat));
		}

		D3D11_TEXTURE3D_DESC desc;
		desc.Width			= static_cast<UINT32>(mWidth);
		desc.Height			= static_cast<UINT32>(mHeight);
		desc.Depth			= static_cast<UINT32>(mDepth);
		desc.Format			= d3dPF;
		desc.MiscFlags		= 0;

		if((mUsage & TU_RENDERTARGET) != 0)
		{
			desc.Usage			= D3D11_USAGE_DEFAULT;
			desc.BindFlags		= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.MipLevels		= 1;
		}
		else if((mUsage & TU_DEPTHSTENCIL) != 0)
		{
			desc.Usage			= D3D11_USAGE_DEFAULT;
			desc.BindFlags		= D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.MipLevels		= 1;
		}
		else
		{
			desc.Usage			= D3D11Mappings::_getUsage(mUsage);
			desc.BindFlags		= D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = D3D11Mappings::_getAccessFlags(mUsage);

			// Determine total number of mipmaps including main one (d3d11 convention)
			UINT numMips = (mNumMipmaps == MIP_UNLIMITED || (1U << mNumMipmaps) > std::max(std::max(mWidth, mHeight), mDepth)) ? 0 : mNumMipmaps + 1;
			desc.MipLevels		= numMips;
		}

		// Create the texture
		D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		D3D11Device& device = rs->getPrimaryDevice();
		hr = device.getD3D11Device()->CreateTexture3D(&desc, nullptr, &m3DTex);

		// Check result and except if failed
		if (FAILED(hr) || device.hasError())
		{
			destroy_internal();
			String errorDescription = device.getErrorDescription();
			BS_EXCEPT(RenderingAPIException, "Error creating texture\nError Description:" + errorDescription);
		}

		hr = m3DTex->QueryInterface(__uuidof(ID3D11Resource), (void **)&mTex);

		if(FAILED(hr) || device.hasError())
		{
			destroy_internal();
			String errorDescription = device.getErrorDescription();
			BS_EXCEPT(RenderingAPIException, "Can't get base texture\nError Description:" + errorDescription);
		}

		// Create texture view
		m3DTex->GetDesc(&desc);

		if(mNumMipmaps != (desc.MipLevels - 1))
		{
			BS_EXCEPT(RenderingAPIException, "Driver returned different number of mip maps than requested. " \
               "Requested: " + toString(mNumMipmaps) + ". Got: " + toString(desc.MipLevels - 1) + ".");
		}

		mDimension = D3D11_SRV_DIMENSION_TEXTURE3D;

		if((mUsage & TU_DEPTHSTENCIL) == 0)
		{
			ZeroMemory(&mSRVDesc, sizeof(mSRVDesc));
			mSRVDesc.Format = desc.Format;
			mSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
			mSRVDesc.Texture3D.MostDetailedMip = 0;
			mSRVDesc.Texture3D.MipLevels = desc.MipLevels;

			hr = device.getD3D11Device()->CreateShaderResourceView(m2DTex, &mSRVDesc, &mShaderResourceView);

			if (FAILED(hr) || device.hasError())
			{
				String errorDescription = device.getErrorDescription();
				BS_EXCEPT(RenderingAPIException, "D3D11 device can't create shader resource view.\nError Description:" + errorDescription);
			}
		}
	}

	void* D3D11Texture::map(ID3D11Resource* res, D3D11_MAP flags, UINT32 mipLevel, UINT32 face, UINT32& rowPitch, UINT32& slicePitch)
	{
		D3D11_MAPPED_SUBRESOURCE pMappedResource;
		pMappedResource.pData = nullptr;

		mipLevel = Math::clamp(mipLevel, (UINT32)mipLevel, getNumMipmaps());
		face = Math::clamp(face, (UINT32)0, mDepth - 1);

		if(getTextureType() == TEX_TYPE_3D)
			face = 0;

		D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		D3D11Device& device = rs->getPrimaryDevice();

		mLockedSubresourceIdx = D3D11CalcSubresource(mipLevel, face, getNumMipmaps()+1);
		device.getImmediateContext()->Map(res, mLockedSubresourceIdx, flags, 0, &pMappedResource);

		if (device.hasError())
		{
			String errorDescription = device.getErrorDescription();
			BS_EXCEPT(RenderingAPIException, "D3D11 device cannot map texture\nError Description:" + errorDescription);
		}

		UINT32 bytesPerPixel = PixelUtil::getNumElemBytes(getFormat());
		rowPitch = pMappedResource.RowPitch / bytesPerPixel;
		slicePitch = pMappedResource.DepthPitch / bytesPerPixel;

		return pMappedResource.pData;
	}

	void D3D11Texture::unmap(ID3D11Resource* res)
	{
		D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		D3D11Device& device = rs->getPrimaryDevice();
		device.getImmediateContext()->Unmap(res, mLockedSubresourceIdx);

		if (device.hasError())
		{
			String errorDescription = device.getErrorDescription();
			BS_EXCEPT(RenderingAPIException, "D3D11 device unmap resource\nError Description:" + errorDescription);
		}
	}

	void* D3D11Texture::mapstagingbuffer(D3D11_MAP flags, UINT32 mipLevel, UINT32 face, UINT32& rowPitch, UINT32& slicePitch)
	{
		if(!mStagingBuffer)
			createStagingBuffer();

		D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		D3D11Device& device = rs->getPrimaryDevice();
		device.getImmediateContext()->CopyResource(mStagingBuffer, mTex);

		return map(mStagingBuffer, flags, face, mipLevel, rowPitch, slicePitch);
	}

	void D3D11Texture::unmapstagingbuffer()
	{
		unmap(mStagingBuffer);
	}

	void* D3D11Texture::mapstaticbuffer(PixelData lock, UINT32 mipLevel, UINT32 face)
	{
		UINT32 sizeOfImage = lock.getConsecutiveSize();
		mLockedSubresourceIdx = D3D11CalcSubresource(mipLevel, face, getNumMipmaps()+1);

		mStaticBuffer = bs_new<PixelData, PoolAlloc>(lock.getWidth(), lock.getHeight(), lock.getDepth(), lock.getFormat());
		mStaticBuffer->allocateInternalBuffer();

		return mStaticBuffer->getData();
	}

	void D3D11Texture::unmapstaticbuffer()
	{
		UINT32 rowWidth = D3D11Mappings::_getSizeInBytes(mStaticBuffer->getFormat(), mStaticBuffer->getWidth());
		UINT32 sliceWidth = D3D11Mappings::_getSizeInBytes(mStaticBuffer->getFormat(), mStaticBuffer->getWidth(), mStaticBuffer->getHeight());

		D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		D3D11Device& device = rs->getPrimaryDevice();
		device.getImmediateContext()->UpdateSubresource(mTex, mLockedSubresourceIdx, nullptr, mStaticBuffer->getData(), rowWidth, sliceWidth);

		if (device.hasError())
		{
			String errorDescription = device.getErrorDescription();
			BS_EXCEPT(RenderingAPIException, "D3D11 device cannot map texture\nError Description:" + errorDescription);
		}

		if(mStaticBuffer != nullptr)
			bs_delete<PoolAlloc>(mStaticBuffer);
	}

	void D3D11Texture::createStagingBuffer()
	{
		D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		D3D11Device& device = rs->getPrimaryDevice();
		switch (getTextureType())
		{
		case TEX_TYPE_1D:
			{
				D3D11_TEXTURE1D_DESC desc;
				m1DTex->GetDesc(&desc);

				desc.BindFlags = 0;
				desc.MiscFlags = 0;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
				desc.Usage = D3D11_USAGE_STAGING;

				device.getD3D11Device()->CreateTexture1D(&desc, NULL, (ID3D11Texture1D**)(&mStagingBuffer));
			} 					
			break;
		case TEX_TYPE_2D:
		case TEX_TYPE_CUBE_MAP:
			{
				D3D11_TEXTURE2D_DESC desc;
				m2DTex->GetDesc(&desc);

				desc.BindFlags = 0;
				desc.MiscFlags = 0;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
				desc.Usage = D3D11_USAGE_STAGING;

				device.getD3D11Device()->CreateTexture2D(&desc, NULL, (ID3D11Texture2D**)(&mStagingBuffer));
			}
			break;
		case TEX_TYPE_3D:
			{
				D3D11_TEXTURE3D_DESC desc;
				m3DTex->GetDesc(&desc);

				desc.BindFlags = 0;
				desc.MiscFlags = 0;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
				desc.Usage = D3D11_USAGE_STAGING;

				device.getD3D11Device()->CreateTexture3D(&desc, NULL, (ID3D11Texture3D**)(&mStagingBuffer));
			}
			break;
		}
	}

	TextureViewPtr D3D11Texture::createView()
	{
		TextureViewPtr viewPtr = bs_core_ptr<D3D11TextureView, PoolAlloc>(new (bs_alloc<D3D11TextureView, PoolAlloc>()) D3D11TextureView());
		viewPtr->_setThisPtr(viewPtr);

		return viewPtr;
	}
}
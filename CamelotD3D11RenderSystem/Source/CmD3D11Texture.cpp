#include "CmD3D11Texture.h"
#include "CmD3D11Mappings.h"
#include "CmD3D11Device.h"
#include "CmD3D11RenderSystem.h"
#include "CmException.h"
#include "CmAsyncOp.h"

#if CM_DEBUG_MODE
#define THROW_IF_NOT_RENDER_THREAD throwIfNotRenderThread();
#else
#define THROW_IF_NOT_RENDER_THREAD 
#endif

namespace CamelotEngine
{
	D3D11Texture::D3D11Texture()
		: m1DTex(nullptr)
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
		THROW_IF_NOT_RENDER_THREAD;

		freeInternalResources();			
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
			CM_EXCEPT(RenderingAPIException, "D3D11 device cannot copy resource\nError Description:" + errorDescription);
		}
	}

	PixelData D3D11Texture::lockImpl(GpuLockOptions options, UINT32 mipLevel, UINT32 face)
	{
		UINT32 mipWidth = mipLevel >> mWidth;
		UINT32 mipHeight = mipLevel >> mHeight;
		UINT32 mipDepth = mipLevel >> mDepth;

		PixelData lockedArea(mipWidth, mipHeight, mipDepth, mFormat);

		D3D11_MAP flags = D3D11Mappings::_getLockOptions(options);
		switch(options)
		{
		case GBL_WRITE_ONLY_NO_OVERWRITE:
			flags = D3D11_MAP_WRITE_NO_OVERWRITE;
			break;
		case GBL_READ_WRITE:
			flags = D3D11_MAP_READ_WRITE;
			break;
		case GBL_WRITE_ONLY_DISCARD:
			flags = D3D11_MAP_WRITE_DISCARD;
			break;
		case GBL_READ_ONLY:
			flags = D3D11_MAP_READ;
			break;
		case GBL_WRITE_ONLY:
			flags = D3D11_MAP_WRITE;
			break;
		default: 
			break;
		};

		if(flags == D3D11_MAP_READ || flags == D3D11_MAP_READ_WRITE)
		{
			lockedArea.data = _mapstagingbuffer(flags, face, mipLevel);
			mLockedForReading = true;
		}
		else
		{
			if(mUsage == TU_DYNAMIC)
				lockedArea.data = _map(mTex, flags, face, mipLevel);
			else
				lockedArea.data = _mapstaticbuffer(lockedArea, mipLevel, face);

			mLockedForReading = false;
		}

		return lockedArea;
	}

	void D3D11Texture::unlockImpl()
	{
		if(mLockedForReading)
			_unmapstagingbuffer();
		else
		{
			if(mUsage == TU_DYNAMIC)
				_unmap(mTex);
			else
				_unmapstaticbuffer();
		}
	}

	void D3D11Texture::initialize_internal()
	{
		THROW_IF_NOT_RENDER_THREAD

		createInternalResources();

		Resource::initialize_internal();
	}

	void D3D11Texture::createInternalResourcesImpl()
	{
		// load based on tex.type
		switch (getTextureType())
		{
		case TEX_TYPE_1D:
			_create1DTex();
			break;
		case TEX_TYPE_2D:
		case TEX_TYPE_CUBE_MAP:
			_create2DTex();
			break;
		case TEX_TYPE_3D:
			_create3DTex();
			break;
		default:
			freeInternalResources();
			CM_EXCEPT(RenderingAPIException, "Unknown texture type");
		}
	}

	void D3D11Texture::freeInternalResourcesImpl()
	{
		SAFE_RELEASE(mTex);
		SAFE_RELEASE(mShaderResourceView);
		SAFE_RELEASE(m1DTex);
		SAFE_RELEASE(m2DTex);
		SAFE_RELEASE(m3DTex);
		SAFE_RELEASE(mStagingBuffer);
	}

	void D3D11Texture::_create1DTex()
	{
		// We must have those defined here
		assert(mWidth > 0);

		// Determine which D3D11 pixel format we'll use
		HRESULT hr;
		DXGI_FORMAT d3dPF = D3D11Mappings::_getPF(D3D11Mappings::_getClosestSupportedPF(mFormat));

		D3D11_TEXTURE1D_DESC desc;
		desc.Width			= static_cast<UINT32>(mWidth);
		desc.ArraySize		= 1;
		desc.Format			= d3dPF;
		desc.MiscFlags		= 0;

		if((mUsage & TU_RENDERTARGET) != 0)
		{
			desc.Usage			= D3D11_USAGE_DEFAULT;
			desc.BindFlags		= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = (mUsage & TU_RENDERTARGET) != 0 ? 0 : D3D11Mappings::_getAccessFlags(mUsage);
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
			freeInternalResources();
			String errorDescription = device.getErrorDescription();
			CM_EXCEPT(RenderingAPIException, "Error creating texture\nError Description:" + errorDescription);
		}

		hr = m1DTex->QueryInterface(__uuidof(ID3D11Resource), (void **)mTex);

		if(FAILED(hr) || device.hasError())
		{
			freeInternalResources();
			String errorDescription = device.getErrorDescription();
			CM_EXCEPT(RenderingAPIException, "Can't get base texture\nError Description:" + errorDescription);
		}

		m1DTex->GetDesc(&desc);
		mNumMipmaps = desc.MipLevels - 1;

		// Create texture view
		ZeroMemory(&mSRVDesc, sizeof(mSRVDesc));
		mSRVDesc.Format = desc.Format;
		mSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D; 
		mSRVDesc.Texture1D.MipLevels = desc.MipLevels;
		hr = device.getD3D11Device()->CreateShaderResourceView(m1DTex, &mSRVDesc, &mShaderResourceView);

		if (FAILED(hr) || device.hasError())
		{
			String errorDescription = device.getErrorDescription();
			CM_EXCEPT(RenderingAPIException, "D3D11 device can't create shader resource view.\nError Description:" + errorDescription);
		}
	}

	void D3D11Texture::_create2DTex()
	{
		// We must have those defined here
		assert(mWidth > 0 || mHeight > 0);

		// Determine which D3D11 pixel format we'll use
		HRESULT hr;
		DXGI_FORMAT d3dPF = D3D11Mappings::_getPF(D3D11Mappings::_getClosestSupportedPF(mFormat));

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
			desc.CPUAccessFlags = (mUsage & TU_RENDERTARGET) != 0 ? 0 : D3D11Mappings::_getAccessFlags(mUsage);
			desc.MipLevels		= 1;

			DXGI_SAMPLE_DESC sampleDesc;
			D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
			rs->determineFSAASettings(mFSAA, mFSAAHint, d3dPF, &sampleDesc);
			desc.SampleDesc		= sampleDesc;

			if(getTextureType() == TEX_TYPE_CUBE_MAP)
			{
				CM_EXCEPT(NotImplementedException, "Cube map not yet supported as a render target."); // TODO: Will be once I add proper texture array support
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
			freeInternalResources();
			String errorDescription = device.getErrorDescription();
			CM_EXCEPT(RenderingAPIException, "Error creating texture\nError Description:" + errorDescription);
		}

		hr = m2DTex->QueryInterface(__uuidof(ID3D11Resource), (void **)mTex);

		if(FAILED(hr) || device.hasError())
		{
			freeInternalResources();
			String errorDescription = device.getErrorDescription();
			CM_EXCEPT(RenderingAPIException, "Can't get base texture\nError Description:" + errorDescription);
		}

		m2DTex->GetDesc(&desc);
		mNumMipmaps = desc.MipLevels - 1;

		// Create shader texture view
		ZeroMemory(&mSRVDesc, sizeof(mSRVDesc));
		mSRVDesc.Format = desc.Format;

		if((mUsage & TU_RENDERTARGET) != 0)
		{
			if(mFSAA > 0)
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

		hr = device.getD3D11Device()->CreateShaderResourceView(m2DTex, &mSRVDesc, &mShaderResourceView);

		if (FAILED(hr) || device.hasError())
		{
			String errorDescription = device.getErrorDescription();
			CM_EXCEPT(RenderingAPIException, "D3D11 device can't create shader resource view.\nError Description:" + errorDescription);
		}
	}

	void D3D11Texture::_create3DTex()
	{
		// We must have those defined here
		assert(mWidth > 0 && mHeight > 0 && mDepth > 0);

		// Determine which D3D11 pixel format we'll use
		HRESULT hr;
		DXGI_FORMAT d3dPF = D3D11Mappings::_getPF(D3D11Mappings::_getClosestSupportedPF(mFormat));

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
			desc.CPUAccessFlags = (mUsage & TU_RENDERTARGET) != 0 ? 0 : D3D11Mappings::_getAccessFlags(mUsage);
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
			freeInternalResources();
			String errorDescription = device.getErrorDescription();
			CM_EXCEPT(RenderingAPIException, "Error creating texture\nError Description:" + errorDescription);
		}

		hr = m3DTex->QueryInterface(__uuidof(ID3D11Resource), (void **)mTex);

		if(FAILED(hr) || device.hasError())
		{
			freeInternalResources();
			String errorDescription = device.getErrorDescription();
			CM_EXCEPT(RenderingAPIException, "Can't get base texture\nError Description:" + errorDescription);
		}

		// Create texture view
		m3DTex->GetDesc(&desc);
		mNumMipmaps = desc.MipLevels - 1;

		ZeroMemory(&mSRVDesc, sizeof(mSRVDesc));
		mSRVDesc.Format = desc.Format;
		mSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
		mSRVDesc.Texture3D.MostDetailedMip = 0;
		mSRVDesc.Texture3D.MipLevels = desc.MipLevels;

		hr = device.getD3D11Device()->CreateShaderResourceView(m2DTex, &mSRVDesc, &mShaderResourceView);

		if (FAILED(hr) || device.hasError())
		{
			String errorDescription = device.getErrorDescription();
			CM_EXCEPT(RenderingAPIException, "D3D11 device can't create shader resource view.\nError Description:" + errorDescription);
		}
	}

	void* D3D11Texture::_map(ID3D11Resource* res, D3D11_MAP flags, UINT32 mipLevel, UINT32 face)
	{
		D3D11_MAPPED_SUBRESOURCE pMappedResource;
		pMappedResource.pData = nullptr;

		mipLevel = Math::Clamp(mipLevel, (UINT32)mipLevel, getNumMipmaps());
		face = Math::Clamp(face, (UINT32)0, mDepth - 1);

		if(getTextureType() == TEX_TYPE_3D)
			face = 0;

		D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		D3D11Device& device = rs->getPrimaryDevice();

		mLockedSubresourceIdx = D3D11CalcSubresource(mipLevel, face, getNumMipmaps()+1);
		device.getImmediateContext()->Map(res, mLockedSubresourceIdx, flags, 0, &pMappedResource);

		if (device.hasError())
		{
			String errorDescription = device.getErrorDescription();
			CM_EXCEPT(RenderingAPIException, "D3D11 device cannot map texture\nError Description:" + errorDescription);
		}

		return pMappedResource.pData;
	}

	void D3D11Texture::_unmap(ID3D11Resource* res)
	{
		D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		D3D11Device& device = rs->getPrimaryDevice();
		device.getImmediateContext()->Unmap(res, mLockedSubresourceIdx);

		if (device.hasError())
		{
			String errorDescription = device.getErrorDescription();
			CM_EXCEPT(RenderingAPIException, "D3D11 device unmap resource\nError Description:" + errorDescription);
		}
	}

	void* D3D11Texture::_mapstagingbuffer(D3D11_MAP flags, UINT32 mipLevel, UINT32 face)
	{
		if(!mStagingBuffer)
			_createStagingBuffer();

		D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		D3D11Device& device = rs->getPrimaryDevice();
		device.getImmediateContext()->CopyResource(mStagingBuffer, mTex);

		return _map(mStagingBuffer, flags, face, mipLevel);
	}

	void D3D11Texture::_unmapstagingbuffer()
	{
		_unmap(mStagingBuffer);
	}

	void* D3D11Texture::_mapstaticbuffer(PixelData lock, UINT32 mipLevel, UINT32 face)
	{
		UINT32 sizeOfImage = lock.getConsecutiveSize();
		mLockedSubresourceIdx = D3D11CalcSubresource(mipLevel, face, getNumMipmaps()+1);

		UINT8* bufferData = new UINT8[sizeOfImage];
		mStaticBuffer = new PixelData(lock.getWidth(), lock.getHeight(), lock.getDepth(), lock.getFormat(), bufferData, true);

		return bufferData;
	}

	void D3D11Texture::_unmapstaticbuffer()
	{
		UINT32 rowWidth = D3D11Mappings::_getSizeInBytes(mStaticBuffer->getFormat(), mStaticBuffer->getWidth());
		UINT32 sliceWidth = D3D11Mappings::_getSizeInBytes(mStaticBuffer->getFormat(), mStaticBuffer->getWidth(), mStaticBuffer->getHeight());

		D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		D3D11Device& device = rs->getPrimaryDevice();
		device.getImmediateContext()->UpdateSubresource(mTex, mLockedSubresourceIdx, nullptr, mStaticBuffer->data, rowWidth, sliceWidth);

		if (device.hasError())
		{
			String errorDescription = device.getErrorDescription();
			CM_EXCEPT(RenderingAPIException, "D3D11 device cannot map texture\nError Description:" + errorDescription);
		}

		SAFE_DELETE(mStaticBuffer);
	}

	void D3D11Texture::_createStagingBuffer()
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
		case TEX_TYPE_2D_ARRAY:
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
}

#undef THROW_IF_NOT_RENDER_THREAD
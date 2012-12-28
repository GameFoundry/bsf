#include "CmD3D11RenderTexture.h"
#include "CmDepthStencilBuffer.h"
#include "CmD3D11RenderSystem.h"
#include "CmD3D11Device.h"
#include "CmD3D11Texture.h"
#include "CmD3D11Mappings.h"
#include "CmTextureManager.h"
#include "CmException.h"

namespace CamelotEngine
{
	D3D11RenderTexture::D3D11RenderTexture(TextureType textureType, UINT32 width, UINT32 height, 
		PixelFormat format, bool hwGamma, UINT32 fsaa, const String& fsaaHint,
		bool createDepth, UINT32 depthBits)
		:mRenderTargetView(nullptr)
	{
		mPriority = CM_REND_TO_TEX_RT_GROUP;

		if(textureType != TEX_TYPE_2D)
			CM_EXCEPT(NotImplementedException, "Render textures are currently only implemented for 2D surfaces.");

		mWidth = width;
		mHeight = height;
		mType = textureType;
		mFormat = format;
		mColorDepth = CamelotEngine::PixelUtil::getNumElemBits(format);
		mActive = true;
		mHwGamma = hwGamma;
		mFSAA = fsaa;
		mFSAAHint = fsaaHint;
		mDepthBits = depthBits;

		createTextureBuffer();

		if(createDepth)
			createDepthStencilBuffer();

		createResourceView();
	}

	D3D11RenderTexture::D3D11RenderTexture(TexturePtr texture, DepthStencilBufferPtr depthStencilbuffer)
		:mRenderTargetView(nullptr)
	{
		setBuffers(texture, depthStencilbuffer);
	}

	void D3D11RenderTexture::setBuffers(TexturePtr texture, DepthStencilBufferPtr depthStencilBuffer)
	{
		assert(texture != nullptr);
		assert(depthStencilBuffer != nullptr);

		SAFE_RELEASE(mRenderTargetView)
		mPriority = CM_REND_TO_TEX_RT_GROUP;

		if(texture->getTextureType() != TEX_TYPE_2D)
			CM_EXCEPT(NotImplementedException, "Render textures are currently only implemented for 2D surfaces.");

		if(texture->getWidth() != depthStencilBuffer->getWidth() ||
			texture->getHeight() != depthStencilBuffer->getWidth() ||
			texture->getFSAA() != depthStencilBuffer->getFsaa() ||
			texture->getFSAAHint() != depthStencilBuffer->getFsaaHint())
		{
			String errorInfo = "\nWidth: " + toString(texture->getWidth()) + "/" + toString(depthStencilBuffer->getWidth());
			errorInfo += "\nHeight: " + toString(texture->getHeight()) + "/" + toString(depthStencilBuffer->getHeight());
			errorInfo += "\nFSAA: " + toString(texture->getFSAA()) + "/" + toString(depthStencilBuffer->getFsaa());
			errorInfo += "\nFSAAHint: " + texture->getFSAAHint() + "/" + depthStencilBuffer->getFsaaHint();

			CM_EXCEPT(InvalidParametersException, "Provided texture and depth stencil buffer don't match!" + errorInfo);
		}

		mWidth = texture->getWidth();
		mHeight = texture->getWidth();
		mColorDepth = CamelotEngine::PixelUtil::getNumElemBits(texture->getFormat());
		mActive = true;
		mHwGamma = texture->isHardwareGammaEnabled();
		mFSAA = texture->getFSAA();
		mFSAAHint = texture->getFSAAHint();
		mType = texture->getTextureType();
		mFormat = texture->getFormat();
		mDepthBits = depthStencilBuffer->getBitDepth();

		mTexture = texture;
		mDepthStencilBuffer = depthStencilBuffer;

		createResourceView();
	}

	D3D11RenderTexture::~D3D11RenderTexture()
	{
		SAFE_RELEASE(mRenderTargetView);
	}

	void D3D11RenderTexture::createTextureBuffer()
	{
		mTexture = TextureManager::instance().createTexture(mType, mWidth, mHeight, 0, mFormat, TU_RENDERTARGET, mHwGamma, mFSAA, mFSAAHint);
	}

	void D3D11RenderTexture::createDepthStencilBuffer()
	{
		mDepthStencilBuffer = TextureManager::instance().createDepthStencilBuffer(mDepthBits, mWidth, mHeight, mFSAA, mFSAAHint);
	}

	void D3D11RenderTexture::createResourceView()
	{
		D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
		ZeroMemory(&RTVDesc, sizeof(RTVDesc));

		D3D11Texture* d3d11Texture = static_cast<D3D11Texture*>(mTexture.get());

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = d3d11Texture->getSRVDesc();
		RTVDesc.Format = SRVDesc.Format;

		switch(SRVDesc.ViewDimension)
		{
		case D3D11_SRV_DIMENSION_BUFFER:
			RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_BUFFER;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE1D:
			RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1D;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE1DARRAY:
			RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
			break;
		case D3D11_SRV_DIMENSION_TEXTURECUBE:
			RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			RTVDesc.Texture2DArray.FirstArraySlice = 0;
			RTVDesc.Texture2DArray.ArraySize = 6;
			RTVDesc.Texture2DArray.MipSlice = 0;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE2D:
			RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE2DARRAY:
			RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE2DMS:
			RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY:
			RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE3D:
			RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
			break;
		default:
			assert(false);
		}

		D3D11Device& device = D3D11RenderSystem::getPrimaryDevice();
		HRESULT hr = device.getD3D11Device()->CreateRenderTargetView(d3d11Texture->getDX11Resource(), &RTVDesc, &mRenderTargetView);

		if (FAILED(hr) || device.hasError())
		{
			String errorDescription = device.getErrorDescription();
			CM_EXCEPT(RenderingAPIException, "Error creating Render Target View\nError Description:" + errorDescription);
		}
	}
}
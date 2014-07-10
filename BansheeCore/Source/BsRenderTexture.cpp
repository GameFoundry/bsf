//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsRenderTexture.h"
#include "BsException.h"
#include "BsPixelBuffer.h"
#include "BsTexture.h"
#include "BsTextureManager.h"
#include "BsResources.h"

namespace BansheeEngine
{
	RenderTexture::RenderTexture()
		:mColorSurface(nullptr), mDepthStencilSurface(nullptr)
	{

	}

	RenderTexture::~RenderTexture()
	{

	}

	RenderTexturePtr RenderTexture::create(TextureType textureType, UINT32 width, UINT32 height, 
		PixelFormat format, bool hwGamma, UINT32 multisampleCount, const String& multisampleHint, 
		bool createDepth, PixelFormat depthStencilFormat)
	{
		return TextureManager::instance().createRenderTexture(textureType, width, height, format, hwGamma, 
			multisampleCount, multisampleHint, createDepth, depthStencilFormat);
	}

	void RenderTexture::destroy_internal()
	{
		if(mColorSurface != nullptr)
			Texture::releaseView(mColorSurface);

		if(mDepthStencilSurface != nullptr)
			Texture::releaseView(mDepthStencilSurface);

		RenderTarget::destroy_internal();
	}

	void RenderTexture::initialize(const RENDER_TEXTURE_DESC& desc)
	{
		if(desc.colorSurface.texture != nullptr)
		{
			TexturePtr texture = desc.colorSurface.texture;

			if(texture->getUsage() != TU_RENDERTARGET)
				BS_EXCEPT(InvalidParametersException, "Provided texture is not created with render target usage.");

			mColorSurface = Texture::requestView(texture, desc.colorSurface.mipLevel, 1, 
				desc.colorSurface.face, desc.colorSurface.numFaces, GVU_RENDERTARGET);

			mWidth = texture->getWidth();
			mHeight = texture->getHeight();
			mColorDepth = BansheeEngine::PixelUtil::getNumElemBits(texture->getFormat());
			mActive = true;
			mHwGamma = texture->isHardwareGammaEnabled();
			mMultisampleCount = texture->getMultisampleCount();
			mMultisampleHint = texture->getMultisampleHint();
		}

		if(desc.depthStencilSurface.texture != nullptr)
		{
			TexturePtr texture = desc.depthStencilSurface.texture;

			if(texture->getUsage() != TU_DEPTHSTENCIL)
				BS_EXCEPT(InvalidParametersException, "Provided texture is not created with depth stencil usage.");

			mDepthStencilSurface = Texture::requestView(texture, desc.depthStencilSurface.mipLevel, 1, 
				desc.depthStencilSurface.face, desc.depthStencilSurface.numFaces, GVU_DEPTHSTENCIL);
		}

		throwIfBuffersDontMatch();

		assert(mColorSurface != nullptr);
		assert(mColorSurface->getTexture() != nullptr);

		if(mColorSurface->getTexture()->getTextureType() != TEX_TYPE_2D)
			BS_EXCEPT(NotImplementedException, "Render textures are currently only implemented for 2D surfaces.");

		if((mColorSurface->getFirstArraySlice() + mColorSurface->getNumArraySlices()) > mColorSurface->getTexture()->getNumFaces())
		{
			BS_EXCEPT(InvalidParametersException, "Provided number of faces is out of range. Face: " + 
				toString(mColorSurface->getFirstArraySlice() + mColorSurface->getNumArraySlices()) + 
				". Max num faces: " + toString(mColorSurface->getTexture()->getNumFaces()));
		}

		if(mColorSurface->getMostDetailedMip() > mColorSurface->getTexture()->getNumMipmaps())
		{
			BS_EXCEPT(InvalidParametersException, "Provided number of mip maps is out of range. Mip level: " + 
				toString(mColorSurface->getMostDetailedMip()) + ". Max num mipmaps: " + toString(mColorSurface->getTexture()->getNumMipmaps()));
		}

		RenderTarget::initialize();

		// Create non-persistent resource handles for the used textures (we only need them because a lot of the code accepts only handles,
		// since they're non persistent they don't really have any benefit over shared pointers)
		if(mColorSurface != nullptr)
			mBindableColorTex = gResources()._createResourceHandle(mColorSurface->getTexture());

		if(mDepthStencilSurface != nullptr)
			mBindableDepthStencilTex = gResources()._createResourceHandle(mDepthStencilSurface->getTexture());
	}

	void RenderTexture::throwIfBuffersDontMatch() const
	{
		if(mColorSurface == nullptr || mDepthStencilSurface == nullptr)
			return;

		if(mColorSurface->getTexture()->getWidth() != mDepthStencilSurface->getTexture()->getWidth() ||
			mColorSurface->getTexture()->getHeight() != mDepthStencilSurface->getTexture()->getHeight() ||
			mColorSurface->getTexture()->getMultisampleCount() != mDepthStencilSurface->getTexture()->getMultisampleCount() ||
			mColorSurface->getTexture()->getMultisampleHint() != mDepthStencilSurface->getTexture()->getMultisampleHint())
		{
			String errorInfo = "\nWidth: " + toString(mColorSurface->getTexture()->getWidth()) + "/" + toString(mDepthStencilSurface->getTexture()->getWidth());
			errorInfo += "\nHeight: " + toString(mColorSurface->getTexture()->getHeight()) + "/" + toString(mDepthStencilSurface->getTexture()->getHeight());
			errorInfo += "\nMultisample Count: " + toString(mColorSurface->getTexture()->getMultisampleCount()) + "/" + toString(mDepthStencilSurface->getTexture()->getMultisampleCount());
			errorInfo += "\nMultisample Hint: " + mColorSurface->getTexture()->getMultisampleHint() + "/" + mDepthStencilSurface->getTexture()->getMultisampleHint();

			BS_EXCEPT(InvalidParametersException, "Provided texture and depth stencil buffer don't match!" + errorInfo);
		}
	}

	void RenderTexture::copyToMemory(PixelData &dst, FrameBuffer buffer)
	{
		throw std::exception("The method or operation is not implemented.");
	}
}

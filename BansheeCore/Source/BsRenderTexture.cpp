#include "BsRenderTexture.h"
#include "BsException.h"
#include "BsPixelBuffer.h"
#include "BsTexture.h"
#include "BsTextureManager.h"
#include "BsResources.h"
#include "BsCoreThread.h"

namespace BansheeEngine
{
	RenderTextureCore::RenderTextureCore(RenderTexture* parent, RenderTextureProperties* properties, const RENDER_SURFACE_DESC& colorSurfaceDesc,
		const RENDER_SURFACE_DESC& depthStencilSurfaceDesc)
		:RenderTargetCore(parent, properties), mColorSurface(nullptr), mDepthStencilSurface(nullptr)
	{
		if (colorSurfaceDesc.texture != nullptr)
		{
			TexturePtr texture = colorSurfaceDesc.texture;

			if (texture->getUsage() != TU_RENDERTARGET)
				BS_EXCEPT(InvalidParametersException, "Provided texture is not created with render target usage.");

			mColorSurface = Texture::requestView(texture, colorSurfaceDesc.mipLevel, 1,
				colorSurfaceDesc.face, colorSurfaceDesc.numFaces, GVU_RENDERTARGET);
		}

		if (depthStencilSurfaceDesc.texture != nullptr)
		{
			TexturePtr texture = depthStencilSurfaceDesc.texture;

			if (texture->getUsage() != TU_DEPTHSTENCIL)
				BS_EXCEPT(InvalidParametersException, "Provided texture is not created with depth stencil usage.");

			mDepthStencilSurface = Texture::requestView(texture, depthStencilSurfaceDesc.mipLevel, 1,
				depthStencilSurfaceDesc.face, depthStencilSurfaceDesc.numFaces, GVU_DEPTHSTENCIL);
		}

		throwIfBuffersDontMatch();

		assert(mColorSurface != nullptr);
		assert(mColorSurface->getTexture() != nullptr);

		if (mColorSurface->getTexture()->getTextureType() != TEX_TYPE_2D)
			BS_EXCEPT(NotImplementedException, "Render textures are currently only implemented for 2D surfaces.");

		if ((mColorSurface->getFirstArraySlice() + mColorSurface->getNumArraySlices()) > mColorSurface->getTexture()->getNumFaces())
		{
			BS_EXCEPT(InvalidParametersException, "Provided number of faces is out of range. Face: " +
				toString(mColorSurface->getFirstArraySlice() + mColorSurface->getNumArraySlices()) +
				". Max num faces: " + toString(mColorSurface->getTexture()->getNumFaces()));
		}

		if (mColorSurface->getMostDetailedMip() > mColorSurface->getTexture()->getNumMipmaps())
		{
			BS_EXCEPT(InvalidParametersException, "Provided number of mip maps is out of range. Mip level: " +
				toString(mColorSurface->getMostDetailedMip()) + ". Max num mipmaps: " + toString(mColorSurface->getTexture()->getNumMipmaps()));
		}
	}

	RenderTextureCore::~RenderTextureCore()
	{
		if (mColorSurface != nullptr)
			Texture::releaseView(mColorSurface);

		if (mDepthStencilSurface != nullptr)
			Texture::releaseView(mDepthStencilSurface);
	}

	void RenderTextureCore::throwIfBuffersDontMatch() const
	{
		if (mColorSurface == nullptr || mDepthStencilSurface == nullptr)
			return;

		if (mColorSurface->getTexture()->getWidth() != mDepthStencilSurface->getTexture()->getWidth() ||
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

	RenderTexture* RenderTextureCore::getNonCore() const
	{
		return static_cast<RenderTexture*>(mParent);
	}

	RenderTexturePtr RenderTexture::create(TextureType textureType, UINT32 width, UINT32 height, 
		PixelFormat format, bool hwGamma, UINT32 multisampleCount, const String& multisampleHint, 
		bool createDepth, PixelFormat depthStencilFormat)
	{
		return TextureManager::instance().createRenderTexture(textureType, width, height, format, hwGamma, 
			multisampleCount, multisampleHint, createDepth, depthStencilFormat);
	}

	RenderTexturePtr RenderTexture::create(const RENDER_TEXTURE_DESC& desc)
	{
		return TextureManager::instance().createRenderTexture(desc);
	}

	const RenderTextureProperties& RenderTexture::getProperties() const 
	{ 
		THROW_IF_CORE_THREAD;

		return static_cast<const RenderTextureProperties&>(RenderTarget::getProperties()); 
	}

	RenderTextureCore* RenderTexture::getCore() const 
	{ 
		return static_cast<RenderTextureCore*>(mCore); 
	}

	void RenderTexture::initialize(const RENDER_TEXTURE_DESC& desc)
	{
		mColorSurfaceDesc = desc.colorSurface;
		mDepthStencilSurfaceDesc = desc.depthStencilSurface;

		TexturePtr texture = desc.colorSurface.texture;

		mProperties = createProperties();
		RenderTextureProperties* properties = static_cast<RenderTextureProperties*>(mProperties);
		if (texture != nullptr)
		{
			properties->mWidth = texture->getWidth();
			properties->mHeight = texture->getHeight();
			properties->mColorDepth = BansheeEngine::PixelUtil::getNumElemBits(texture->getFormat());
			properties->mHwGamma = texture->isHardwareGammaEnabled();
			properties->mMultisampleCount = texture->getMultisampleCount();
			properties->mMultisampleHint = texture->getMultisampleHint();
		}

		properties->mActive = true;
		properties->mIsWindow = false;
		properties->mRequiresTextureFlipping = requiresTextureFlipping();

		// Create non-persistent resource handles for the used textures (we only need them because a lot of the code accepts only handles,
		// since they're non persistent they don't really have any benefit over shared pointers)
		if (desc.colorSurface.texture != nullptr)
			mBindableColorTex = static_resource_cast<Texture>(gResources()._createResourceHandle(desc.colorSurface.texture));

		if (desc.depthStencilSurface.texture != nullptr)
			mBindableDepthStencilTex = static_resource_cast<Texture>(gResources()._createResourceHandle(desc.depthStencilSurface.texture));

		RenderTarget::initialize();
	}

	RenderTargetCore* RenderTexture::createCore()
	{
		RenderTextureProperties* coreProperties = bs_new<RenderTextureProperties>();
		RenderTextureProperties* myProperties = static_cast<RenderTextureProperties*>(mProperties);

		*coreProperties = *myProperties;

		return createCore(coreProperties, mColorSurfaceDesc, mDepthStencilSurfaceDesc);
	}
}

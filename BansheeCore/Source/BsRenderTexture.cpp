#include "BsRenderTexture.h"
#include "BsException.h"
#include "BsPixelBuffer.h"
#include "BsTexture.h"
#include "BsTextureManager.h"
#include "BsResources.h"
#include "BsCoreThread.h"
#include "BsFrameAlloc.h"

namespace BansheeEngine
{
	RenderTextureProperties::RenderTextureProperties(const RENDER_TEXTURE_DESC& desc, bool requiresFlipping)
	{
		TexturePtr texture = desc.colorSurface.texture;

		if (texture != nullptr)
		{
			mWidth = texture->getWidth();
			mHeight = texture->getHeight();
			mColorDepth = BansheeEngine::PixelUtil::getNumElemBits(texture->getFormat());
			mHwGamma = texture->isHardwareGammaEnabled();
			mMultisampleCount = texture->getMultisampleCount();
		}

		mActive = true;
		mIsWindow = false;
		mRequiresTextureFlipping = requiresFlipping;
	}

	RenderTextureCore::RenderTextureCore(const RENDER_TEXTURE_DESC& desc)
		:mColorSurface(nullptr), mDepthStencilSurface(nullptr), mDesc(desc)
	{ }

	RenderTextureCore::~RenderTextureCore()
	{ }

	void RenderTextureCore::initialize()
	{
		RenderTargetCore::initialize();

		const RENDER_SURFACE_DESC& colorSurface = mDesc.colorSurface;
		if (colorSurface.texture != nullptr)
		{
			TexturePtr texture = colorSurface.texture;

			if (texture->getUsage() != TU_RENDERTARGET)
				BS_EXCEPT(InvalidParametersException, "Provided texture is not created with render target usage.");

			mColorSurface = Texture::requestView(texture, colorSurface.mipLevel, 1,
				colorSurface.face, 1, GVU_RENDERTARGET);
		}

		const RENDER_SURFACE_DESC& depthStencilSurface = mDesc.depthStencilSurface;
		if (depthStencilSurface.texture != nullptr)
		{
			TexturePtr texture = depthStencilSurface.texture;

			if (texture->getUsage() != TU_DEPTHSTENCIL)
				BS_EXCEPT(InvalidParametersException, "Provided texture is not created with depth stencil usage.");

			mDepthStencilSurface = Texture::requestView(texture, depthStencilSurface.mipLevel, 1,
				depthStencilSurface.face, 1, GVU_DEPTHSTENCIL);
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

	void RenderTextureCore::destroy()
	{
		if (mColorSurface != nullptr)
			Texture::releaseView(mColorSurface);

		if (mDepthStencilSurface != nullptr)
			Texture::releaseView(mDepthStencilSurface);

		RenderTargetCore::destroy();
	}

	CoreSyncData RenderTextureCore::syncFromCore(FrameAlloc* allocator)
	{
		UINT32 size = sizeof(RenderTextureProperties);
		UINT8* buffer = allocator->alloc(size);

		RenderTextureProperties& props = const_cast<RenderTextureProperties&>(getProperties());

		memcpy(buffer, &props, size);
		return CoreSyncData(buffer, size);
	}

	void RenderTextureCore::syncToCore(const CoreSyncData& data)
	{
		RenderTextureProperties& props = const_cast<RenderTextureProperties&>(getProperties());
		props = data.getData<RenderTextureProperties>();
	}

	const RenderTextureProperties& RenderTextureCore::getProperties() const
	{
		return static_cast<const RenderTextureProperties&>(getPropertiesInternal());
	}

	void RenderTextureCore::throwIfBuffersDontMatch() const
	{
		if (mColorSurface == nullptr || mDepthStencilSurface == nullptr)
			return;

		if (mColorSurface->getTexture()->getWidth() != mDepthStencilSurface->getTexture()->getWidth() ||
			mColorSurface->getTexture()->getHeight() != mDepthStencilSurface->getTexture()->getHeight() ||
			mColorSurface->getTexture()->getMultisampleCount() != mDepthStencilSurface->getTexture()->getMultisampleCount())
		{
			String errorInfo = "\nWidth: " + toString(mColorSurface->getTexture()->getWidth()) + "/" + toString(mDepthStencilSurface->getTexture()->getWidth());
			errorInfo += "\nHeight: " + toString(mColorSurface->getTexture()->getHeight()) + "/" + toString(mDepthStencilSurface->getTexture()->getHeight());
			errorInfo += "\nMultisample Count: " + toString(mColorSurface->getTexture()->getMultisampleCount()) + "/" + toString(mDepthStencilSurface->getTexture()->getMultisampleCount());

			BS_EXCEPT(InvalidParametersException, "Provided texture and depth stencil buffer don't match!" + errorInfo);
		}
	}

	RenderTexturePtr RenderTexture::create(TextureType textureType, UINT32 width, UINT32 height, 
		PixelFormat format, bool hwGamma, UINT32 multisampleCount, 
		bool createDepth, PixelFormat depthStencilFormat)
	{
		return TextureManager::instance().createRenderTexture(textureType, width, height, format, hwGamma, 
			multisampleCount, createDepth, depthStencilFormat);
	}

	RenderTexturePtr RenderTexture::create(const RENDER_TEXTURE_DESC& desc)
	{
		return TextureManager::instance().createRenderTexture(desc);
	}

	SPtr<RenderTextureCore> RenderTexture::getCore() const 
	{ 
		return std::static_pointer_cast<RenderTextureCore>(mCoreSpecific); 
	}

	RenderTexture::RenderTexture(const RENDER_TEXTURE_DESC& desc)
	{
		mDesc = desc;

		// Create non-persistent resource handles for the used textures (we only need them because a lot of the code accepts only handles,
		// since they're non persistent they don't really have any benefit over shared pointers)
		if (desc.colorSurface.texture != nullptr)
			mBindableColorTex = static_resource_cast<Texture>(gResources()._createResourceHandle(desc.colorSurface.texture));

		if (desc.depthStencilSurface.texture != nullptr)
			mBindableDepthStencilTex = static_resource_cast<Texture>(gResources()._createResourceHandle(desc.depthStencilSurface.texture));
	}

	SPtr<CoreObjectCore> RenderTexture::createCore() const
	{
		return TextureCoreManager::instance().createRenderTextureInternal(mDesc);
	}

	CoreSyncData RenderTexture::syncToCore(FrameAlloc* allocator)
	{
		UINT32 size = sizeof(RenderTextureProperties);
		UINT8* buffer = allocator->alloc(size);

		RenderTextureProperties& props = const_cast<RenderTextureProperties&>(getProperties());

		memcpy(buffer, &props, size);
		return CoreSyncData(buffer, size);
	}

	void RenderTexture::syncFromCore(const CoreSyncData& data)
	{
		RenderTextureProperties& props = const_cast<RenderTextureProperties&>(getProperties());
		props = data.getData<RenderTextureProperties>();
	}

	const RenderTextureProperties& RenderTexture::getProperties() const
	{
		return static_cast<const RenderTextureProperties&>(getPropertiesInternal());
	}
}

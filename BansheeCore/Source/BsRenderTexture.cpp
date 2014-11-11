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
			const TextureProperties& props = texture->getProperties();

			mWidth = props.getWidth();
			mHeight = props.getHeight();
			mColorDepth = BansheeEngine::PixelUtil::getNumElemBits(props.getFormat());
			mHwGamma = props.isHardwareGammaEnabled();
			mMultisampleCount = props.getMultisampleCount();
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
			SPtr<TextureCore> texture = colorSurface.texture->getCore();

			if (texture->getProperties().getUsage() != TU_RENDERTARGET)
				BS_EXCEPT(InvalidParametersException, "Provided texture is not created with render target usage.");

			mColorSurface = TextureCore::requestView(texture, colorSurface.mipLevel, 1,
				colorSurface.face, 1, GVU_RENDERTARGET);
		}

		const RENDER_SURFACE_DESC& depthStencilSurface = mDesc.depthStencilSurface;
		if (depthStencilSurface.texture != nullptr)
		{
			SPtr<TextureCore> texture = depthStencilSurface.texture->getCore();

			if (texture->getProperties().getUsage() != TU_DEPTHSTENCIL)
				BS_EXCEPT(InvalidParametersException, "Provided texture is not created with depth stencil usage.");

			mDepthStencilSurface = TextureCore::requestView(texture, depthStencilSurface.mipLevel, 1,
				depthStencilSurface.face, 1, GVU_DEPTHSTENCIL);
		}

		throwIfBuffersDontMatch();

		assert(mColorSurface != nullptr);
		assert(mColorSurface->getTexture() != nullptr);

		SPtr<TextureCore> colorTexture = mColorSurface->getTexture();
		const TextureProperties& texProps = colorTexture->getProperties();

		if ((mColorSurface->getFirstArraySlice() + mColorSurface->getNumArraySlices()) > texProps.getNumFaces())
		{
			BS_EXCEPT(InvalidParametersException, "Provided number of faces is out of range. Face: " +
				toString(mColorSurface->getFirstArraySlice() + mColorSurface->getNumArraySlices()) +
				". Max num faces: " + toString(texProps.getNumFaces()));
		}

		if (mColorSurface->getMostDetailedMip() > texProps.getNumMipmaps())
		{
			BS_EXCEPT(InvalidParametersException, "Provided number of mip maps is out of range. Mip level: " +
				toString(mColorSurface->getMostDetailedMip()) + ". Max num mipmaps: " + toString(texProps.getNumMipmaps()));
		}
	}

	void RenderTextureCore::destroy()
	{
		if (mColorSurface != nullptr)
			TextureCore::releaseView(mColorSurface);

		if (mDepthStencilSurface != nullptr)
			TextureCore::releaseView(mDepthStencilSurface);

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

		const TextureProperties& colorProps = mColorSurface->getTexture()->getProperties();
		const TextureProperties& depthProps = mDepthStencilSurface->getTexture()->getProperties();

		if (colorProps.getWidth() != depthProps.getWidth() ||
			colorProps.getHeight() != depthProps.getHeight() ||
			colorProps.getMultisampleCount() != depthProps.getMultisampleCount())
		{
			String errorInfo = "\nWidth: " + toString(colorProps.getWidth()) + "/" + toString(depthProps.getWidth());
			errorInfo += "\nHeight: " + toString(colorProps.getHeight()) + "/" + toString(depthProps.getHeight());
			errorInfo += "\nMultisample Count: " + toString(colorProps.getMultisampleCount()) + "/" + toString(depthProps.getMultisampleCount());

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

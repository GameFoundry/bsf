#include "BsMultiRenderTexture.h"
#include "BsTexture.h"
#include "BsException.h"
#include "BsDebug.h"
#include "BsCoreThread.h"
#include "BsTextureManager.h"
#include "BsFrameAlloc.h"

namespace BansheeEngine
{
	MultiRenderTextureProperties::MultiRenderTextureProperties(const MULTI_RENDER_TEXTURE_DESC& desc)
	{
		for (size_t i = 0; i < desc.colorSurfaces.size(); i++)
		{
			TexturePtr texture = desc.colorSurfaces[i].texture;

			if (texture != nullptr)
			{
				mWidth = texture->getWidth();
				mHeight = texture->getWidth();
				mColorDepth = BansheeEngine::PixelUtil::getNumElemBits(texture->getFormat());
				mActive = true;
				mHwGamma = texture->isHardwareGammaEnabled();
				mMultisampleCount = texture->getMultisampleCount();
				mIsWindow = false;
				mRequiresTextureFlipping = requiresTextureFlipping();

				break;
			}
		}
	}

	MultiRenderTextureCore::MultiRenderTextureCore(const MULTI_RENDER_TEXTURE_DESC& desc)
		:mDesc(desc)
	{ }

	MultiRenderTextureCore::~MultiRenderTextureCore()
	{ }

	void MultiRenderTextureCore::initialize()
	{
		RenderTargetCore::initialize();

		mColorSurfaces.resize(BS_MAX_MULTIPLE_RENDER_TARGETS);

		for (size_t i = 0; i < mDesc.colorSurfaces.size(); i++)
		{
			if (mDesc.colorSurfaces[i].texture != nullptr)
			{
				if (i >= BS_MAX_MULTIPLE_RENDER_TARGETS)
				{
					LOGWRN("Render texture index is larger than the maximum number of supported render targets. Index: " + toString((int)i) +
						". Max. number of render targets: " + toString(BS_MAX_MULTIPLE_RENDER_TARGETS));

					continue;
				}

				TexturePtr texture = mDesc.colorSurfaces[i].texture;

				if (texture->getUsage() != TU_RENDERTARGET)
					BS_EXCEPT(InvalidParametersException, "Provided texture is not created with render target usage.");

				mColorSurfaces[i] = Texture::requestView(texture, mDesc.colorSurfaces[i].mipLevel, 1,
					mDesc.colorSurfaces[i].face, 1, GVU_RENDERTARGET);
			}
		}

		if (mDesc.depthStencilSurface.texture != nullptr)
		{
			TexturePtr texture = mDesc.depthStencilSurface.texture;

			if (texture->getUsage() != TU_DEPTHSTENCIL)
				BS_EXCEPT(InvalidParametersException, "Provided texture is not created with depth stencil usage.");

			mDepthStencilSurface = Texture::requestView(texture, mDesc.depthStencilSurface.mipLevel, 1,
				mDesc.depthStencilSurface.face, 1, GVU_DEPTHSTENCIL);
		}

		throwIfBuffersDontMatch();
	}

	void MultiRenderTextureCore::destroy()
	{
		for (auto iter = mColorSurfaces.begin(); iter != mColorSurfaces.end(); ++iter)
		{
			if (*iter != nullptr)
				Texture::releaseView(*iter);
		}

		if (mDepthStencilSurface != nullptr)
			Texture::releaseView(mDepthStencilSurface);

		RenderTargetCore::destroy();
	}

	CoreSyncData MultiRenderTextureCore::syncFromCore(FrameAlloc* allocator)
	{
		UINT32 size = sizeof(MultiRenderTextureProperties);
		UINT8* buffer = allocator->alloc(size);

		MultiRenderTextureProperties& props = const_cast<MultiRenderTextureProperties&>(getProperties());

		memcpy(buffer, &props, size);
		return CoreSyncData(buffer, size);
	}

	void MultiRenderTextureCore::syncToCore(const CoreSyncData& data)
	{
		MultiRenderTextureProperties& props = const_cast<MultiRenderTextureProperties&>(getProperties());
		props = data.getData<MultiRenderTextureProperties>();
	}

	const MultiRenderTextureProperties& MultiRenderTextureCore::getProperties() const
	{
		return static_cast<const MultiRenderTextureProperties&>(getPropertiesInternal());
	}

	void MultiRenderTextureCore::throwIfBuffersDontMatch() const
	{
		TextureViewPtr firstSurfaceDesc = nullptr;
		for(size_t i = 0; i < mColorSurfaces.size(); i++)
		{
			if(mColorSurfaces[i] == nullptr)
				continue;

			if(firstSurfaceDesc == nullptr)
			{
				firstSurfaceDesc = mColorSurfaces[i];
				continue;
			}

			if(mColorSurfaces[i]->getTexture()->getWidth() != firstSurfaceDesc->getTexture()->getWidth() ||
				mColorSurfaces[i]->getTexture()->getHeight() != firstSurfaceDesc->getTexture()->getHeight() ||
				mColorSurfaces[i]->getTexture()->getMultisampleCount() != firstSurfaceDesc->getTexture()->getMultisampleCount())
			{
				String errorInfo = "\nWidth: " + toString(mColorSurfaces[i]->getTexture()->getWidth()) + "/" + toString(firstSurfaceDesc->getTexture()->getWidth());
				errorInfo += "\nHeight: " + toString(mColorSurfaces[i]->getTexture()->getHeight()) + "/" + toString(firstSurfaceDesc->getTexture()->getHeight());
				errorInfo += "\nMultisample Count: " + toString(mColorSurfaces[i]->getTexture()->getMultisampleCount()) + "/" + toString(firstSurfaceDesc->getTexture()->getMultisampleCount());

				BS_EXCEPT(InvalidParametersException, "Provided texture and depth stencil buffer don't match!" + errorInfo);
			}
		}

		if(firstSurfaceDesc == nullptr)
			return;

		if(firstSurfaceDesc->getTexture()->getTextureType() != TEX_TYPE_2D)
			BS_EXCEPT(NotImplementedException, "Render textures are currently only implemented for 2D surfaces.");

		if((firstSurfaceDesc->getFirstArraySlice() + firstSurfaceDesc->getNumArraySlices()) >= firstSurfaceDesc->getTexture()->getNumFaces())
		{
			BS_EXCEPT(InvalidParametersException, "Provided number of faces is out of range. Face: " + 
				toString(firstSurfaceDesc->getFirstArraySlice() + firstSurfaceDesc->getNumArraySlices()) + ". Max num faces: " + toString(firstSurfaceDesc->getTexture()->getNumFaces()));
		}

		if(firstSurfaceDesc->getMostDetailedMip() >= firstSurfaceDesc->getTexture()->getNumMipmaps())
		{
			BS_EXCEPT(InvalidParametersException, "Provided number of mip maps is out of range. Mip level: " + 
				toString(firstSurfaceDesc->getMostDetailedMip()) + ". Max num mipmaps: " + toString(firstSurfaceDesc->getTexture()->getNumMipmaps()));
		}

		if(mDepthStencilSurface == nullptr)
			return;

		if(mDepthStencilSurface->getTexture()->getWidth() != firstSurfaceDesc->getTexture()->getWidth() ||
			mDepthStencilSurface->getTexture()->getHeight() != firstSurfaceDesc->getTexture()->getHeight() ||
			mDepthStencilSurface->getTexture()->getMultisampleCount() != firstSurfaceDesc->getTexture()->getMultisampleCount())
		{
			String errorInfo = "\nWidth: " + toString(mDepthStencilSurface->getTexture()->getWidth()) + "/" + toString(firstSurfaceDesc->getTexture()->getWidth());
			errorInfo += "\nHeight: " + toString(mDepthStencilSurface->getTexture()->getHeight()) + "/" + toString(firstSurfaceDesc->getTexture()->getHeight());
			errorInfo += "\nMultisample Count: " + toString(mDepthStencilSurface->getTexture()->getMultisampleCount()) + "/" + toString(firstSurfaceDesc->getTexture()->getMultisampleCount());

			BS_EXCEPT(InvalidParametersException, "Provided texture and depth stencil buffer don't match!" + errorInfo);
		}
	}

	void MultiRenderTextureCore::copyToMemory(PixelData &dst, FrameBuffer buffer)
	{
		throw std::exception("The method or operation is not implemented.");
	}

	MultiRenderTexture::MultiRenderTexture(const MULTI_RENDER_TEXTURE_DESC& desc)
		:mDesc(desc)
	{

	}

	SPtr<CoreObjectCore> MultiRenderTexture::createCore() const
	{
		return TextureCoreManager::instance().createMultiRenderTextureInternal(mDesc);
	}

	SPtr<MultiRenderTextureCore> MultiRenderTexture::getCore() const
	{
		return std::static_pointer_cast<MultiRenderTextureCore>(mCoreSpecific);
	}

	MultiRenderTexturePtr MultiRenderTexture::create(const MULTI_RENDER_TEXTURE_DESC& desc)
	{
		return TextureManager::instance().createMultiRenderTexture(desc);
	}

	CoreSyncData MultiRenderTexture::syncToCore(FrameAlloc* allocator)
	{
		UINT32 size = sizeof(MultiRenderTextureProperties);
		UINT8* buffer = allocator->alloc(size);

		MultiRenderTextureProperties& props = const_cast<MultiRenderTextureProperties&>(getProperties());

		memcpy(buffer, &props, size);
		return CoreSyncData(buffer, size);
	}

	void MultiRenderTexture::syncFromCore(const CoreSyncData& data)
	{
		MultiRenderTextureProperties& props = const_cast<MultiRenderTextureProperties&>(getProperties());
		props = data.getData<MultiRenderTextureProperties>();
	}

	const MultiRenderTextureProperties& MultiRenderTexture::getProperties() const
	{
		return static_cast<const MultiRenderTextureProperties&>(getPropertiesInternal());
	}
}
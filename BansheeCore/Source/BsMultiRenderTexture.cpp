#include "BsMultiRenderTexture.h"
#include "BsTexture.h"
#include "BsException.h"
#include "BsDebug.h"
#include "BsCoreThread.h"
#include "BsTextureManager.h"

namespace BansheeEngine
{
	MultiRenderTextureCore::MultiRenderTextureCore(MultiRenderTexture* parent, MultiRenderTextureProperties* properties, const MULTI_RENDER_TEXTURE_DESC& desc)
		:RenderTargetCore(parent, properties)
	{
		mColorSurfaces.resize(BS_MAX_MULTIPLE_RENDER_TARGETS);

		for (size_t i = 0; i < desc.colorSurfaces.size(); i++)
		{
			if (desc.colorSurfaces[i].texture != nullptr)
			{
				if (i >= BS_MAX_MULTIPLE_RENDER_TARGETS)
				{
					LOGWRN("Render texture index is larger than the maximum number of supported render targets. Index: " + toString((int)i) +
						". Max. number of render targets: " + toString(BS_MAX_MULTIPLE_RENDER_TARGETS));

					continue;
				}

				TexturePtr texture = desc.colorSurfaces[i].texture;

				if (texture->getUsage() != TU_RENDERTARGET)
					BS_EXCEPT(InvalidParametersException, "Provided texture is not created with render target usage.");

				mColorSurfaces[i] = Texture::requestView(texture, desc.colorSurfaces[i].mipLevel, 1,
					desc.colorSurfaces[i].face, desc.colorSurfaces[i].numFaces, GVU_RENDERTARGET);
			}
		}

		if (desc.depthStencilSurface.texture != nullptr)
		{
			TexturePtr texture = desc.depthStencilSurface.texture;

			if (texture->getUsage() != TU_DEPTHSTENCIL)
				BS_EXCEPT(InvalidParametersException, "Provided texture is not created with depth stencil usage.");

			mDepthStencilSurface = Texture::requestView(texture, desc.depthStencilSurface.mipLevel, 1,
				desc.depthStencilSurface.face, desc.depthStencilSurface.numFaces, GVU_DEPTHSTENCIL);
		}

		throwIfBuffersDontMatch();
	}

	MultiRenderTextureCore::~MultiRenderTextureCore()
	{
		for (auto iter = mColorSurfaces.begin(); iter != mColorSurfaces.end(); ++iter)
		{
			if (*iter != nullptr)
				Texture::releaseView(*iter);
		}

		if (mDepthStencilSurface != nullptr)
			Texture::releaseView(mDepthStencilSurface);
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
				mColorSurfaces[i]->getTexture()->getMultisampleCount() != firstSurfaceDesc->getTexture()->getMultisampleCount() ||
				mColorSurfaces[i]->getTexture()->getMultisampleHint() != firstSurfaceDesc->getTexture()->getMultisampleHint())
			{
				String errorInfo = "\nWidth: " + toString(mColorSurfaces[i]->getTexture()->getWidth()) + "/" + toString(firstSurfaceDesc->getTexture()->getWidth());
				errorInfo += "\nHeight: " + toString(mColorSurfaces[i]->getTexture()->getHeight()) + "/" + toString(firstSurfaceDesc->getTexture()->getHeight());
				errorInfo += "\nMultisample Count: " + toString(mColorSurfaces[i]->getTexture()->getMultisampleCount()) + "/" + toString(firstSurfaceDesc->getTexture()->getMultisampleCount());
				errorInfo += "\nMultisample Hint: " + mColorSurfaces[i]->getTexture()->getMultisampleHint() + "/" + firstSurfaceDesc->getTexture()->getMultisampleHint();

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
			mDepthStencilSurface->getTexture()->getMultisampleCount() != firstSurfaceDesc->getTexture()->getMultisampleCount() ||
			mDepthStencilSurface->getTexture()->getMultisampleHint() != firstSurfaceDesc->getTexture()->getMultisampleHint())
		{
			String errorInfo = "\nWidth: " + toString(mDepthStencilSurface->getTexture()->getWidth()) + "/" + toString(firstSurfaceDesc->getTexture()->getWidth());
			errorInfo += "\nHeight: " + toString(mDepthStencilSurface->getTexture()->getHeight()) + "/" + toString(firstSurfaceDesc->getTexture()->getHeight());
			errorInfo += "\nMultisample Count: " + toString(mDepthStencilSurface->getTexture()->getMultisampleCount()) + "/" + toString(firstSurfaceDesc->getTexture()->getMultisampleCount());
			errorInfo += "\nMultisample Hint: " + mDepthStencilSurface->getTexture()->getMultisampleHint() + "/" + firstSurfaceDesc->getTexture()->getMultisampleHint();

			BS_EXCEPT(InvalidParametersException, "Provided texture and depth stencil buffer don't match!" + errorInfo);
		}
	}

	void MultiRenderTextureCore::copyToMemory(PixelData &dst, FrameBuffer buffer)
	{
		throw std::exception("The method or operation is not implemented.");
	}

	MultiRenderTexture* MultiRenderTextureCore::getNonCore() const
	{
		return static_cast<MultiRenderTexture*>(mParent);
	}

	void MultiRenderTexture::initialize(const MULTI_RENDER_TEXTURE_DESC& desc)
	{
		mDesc = desc;

		mProperties = createProperties();
		MultiRenderTextureProperties* properties = static_cast<MultiRenderTextureProperties*>(mProperties);
		for (size_t i = 0; i < desc.colorSurfaces.size(); i++)
		{
			TexturePtr texture = desc.colorSurfaces[i].texture;

			if (texture != nullptr)
			{
				properties->mWidth = texture->getWidth();
				properties->mHeight = texture->getWidth();
				properties->mColorDepth = BansheeEngine::PixelUtil::getNumElemBits(texture->getFormat());
				properties->mActive = true;
				properties->mHwGamma = texture->isHardwareGammaEnabled();
				properties->mMultisampleCount = texture->getMultisampleCount();
				properties->mMultisampleHint = texture->getMultisampleHint();
				properties->mIsWindow = false;
				properties->mRequiresTextureFlipping = requiresTextureFlipping();

				break;
			}
		}

		RenderTarget::initialize();
	}

	const MultiRenderTextureProperties& MultiRenderTexture::getProperties() const
	{
		THROW_IF_CORE_THREAD;

		return static_cast<const MultiRenderTextureProperties&>(RenderTarget::getProperties());
	}

	MultiRenderTextureCore* MultiRenderTexture::getCore() const
	{
		return static_cast<MultiRenderTextureCore*>(mCore);
	}

	RenderTargetCore* MultiRenderTexture::createCore()
	{
		MultiRenderTextureProperties* coreProperties = bs_new<MultiRenderTextureProperties>();
		MultiRenderTextureProperties* myProperties = static_cast<MultiRenderTextureProperties*>(mProperties);

		*coreProperties = *myProperties;

		return createCore(coreProperties, mDesc);
	}

	MultiRenderTexturePtr MultiRenderTexture::create(const MULTI_RENDER_TEXTURE_DESC& desc)
	{
		return TextureManager::instance().createMultiRenderTexture(desc);
	}
}
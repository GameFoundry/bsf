//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "RenderAPI/BsRenderTexture.h"
#include "Error/BsException.h"
#include "Image/BsTexture.h"
#include "Managers/BsTextureManager.h"
#include "Resources/BsResources.h"
#include "CoreThread/BsCoreThread.h"
#include <Private/RTTI/BsRenderTargetRTTI.h>

namespace bs
{
	RenderTextureProperties::RenderTextureProperties(const RENDER_TEXTURE_DESC& desc, bool requiresFlipping)
	{
		UINT32 firstIdx = (UINT32)-1;
		bool requiresHwGamma = false;
		for (UINT32 i = 0; i < BS_MAX_MULTIPLE_RENDER_TARGETS; i++)
		{
			HTexture texture = desc.colorSurfaces[i].texture;

			if (!texture.isLoaded())
				continue;

			if (firstIdx == (UINT32)-1)
				firstIdx = i;

			requiresHwGamma |= texture->getProperties().isHardwareGammaEnabled();
		}

		if (firstIdx == (UINT32)-1)
		{
			HTexture texture = desc.depthStencilSurface.texture;
			if (texture.isLoaded())
			{
				const TextureProperties& texProps = texture->getProperties();
				construct(&texProps, desc.depthStencilSurface.numFaces, desc.depthStencilSurface.mipLevel,
						  requiresFlipping, false);
			}
		}
		else
		{
			HTexture texture = desc.colorSurfaces[firstIdx].texture;

			const TextureProperties& texProps = texture->getProperties();
			construct(&texProps, desc.colorSurfaces[firstIdx].numFaces, desc.colorSurfaces[firstIdx].mipLevel,
					  requiresFlipping, requiresHwGamma);
		}
	}

	RenderTextureProperties::RenderTextureProperties(const ct::RENDER_TEXTURE_DESC& desc, bool requiresFlipping)
	{
		UINT32 firstIdx = (UINT32)-1;
		bool requiresHwGamma = false;
		for (UINT32 i = 0; i < BS_MAX_MULTIPLE_RENDER_TARGETS; i++)
		{
			SPtr<ct::Texture> texture = desc.colorSurfaces[i].texture;

			if (texture == nullptr)
				continue;

			if (firstIdx == (UINT32)-1)
				firstIdx = i;

			requiresHwGamma |= texture->getProperties().isHardwareGammaEnabled();
		}

		if(firstIdx == (UINT32)-1)
		{
			SPtr<ct::Texture> texture = desc.depthStencilSurface.texture;
			if(texture != nullptr)
			{
				const TextureProperties& texProps = texture->getProperties();
				construct(&texProps, desc.depthStencilSurface.numFaces, desc.depthStencilSurface.mipLevel,
						  requiresFlipping, false);
			}
		}
		else
		{
			SPtr<ct::Texture> texture = desc.colorSurfaces[firstIdx].texture;

			const TextureProperties& texProps = texture->getProperties();
			construct(&texProps, desc.colorSurfaces[firstIdx].numFaces, desc.colorSurfaces[firstIdx].mipLevel,
					  requiresFlipping, requiresHwGamma);
		}
	}

	void RenderTextureProperties::construct(const TextureProperties* textureProps, UINT32 numSlices,
											UINT32 mipLevel, bool requiresFlipping, bool hwGamma)
	{
		if (textureProps != nullptr)
		{
			PixelUtil::getSizeForMipLevel(textureProps->getWidth(), textureProps->getHeight(), textureProps->getDepth(),
										  mipLevel, width, height, numSlices);

			numSlices *= numSlices;
			multisampleCount = textureProps->getNumSamples();
		}

		isWindow = false;
		requiresTextureFlipping = requiresFlipping;
		this->hwGamma = hwGamma;
	}

	SPtr<RenderTexture> RenderTexture::create(const TEXTURE_DESC& desc,
		bool createDepth, PixelFormat depthStencilFormat)
	{
		return TextureManager::instance().createRenderTexture(desc, createDepth, depthStencilFormat);
	}

	SPtr<RenderTexture> RenderTexture::create(const RENDER_TEXTURE_DESC& desc)
	{
		return TextureManager::instance().createRenderTexture(desc);
	}

	SPtr<ct::RenderTexture> RenderTexture::getCore() const
	{
		return std::static_pointer_cast<ct::RenderTexture>(mCoreSpecific);
	}

	RenderTexture::RenderTexture(const RENDER_TEXTURE_DESC& desc)
		:mDesc(desc)
	{
		for (UINT32 i = 0; i < BS_MAX_MULTIPLE_RENDER_TARGETS; i++)
		{
			if (desc.colorSurfaces[i].texture != nullptr)
				mBindableColorTex[i] = desc.colorSurfaces[i].texture;
		}

		if (desc.depthStencilSurface.texture != nullptr)
			mBindableDepthStencilTex = desc.depthStencilSurface.texture;
	}

	SPtr<ct::CoreObject> RenderTexture::createCore() const
	{
		ct::RENDER_TEXTURE_DESC coreDesc;

		for (UINT32 i = 0; i < BS_MAX_MULTIPLE_RENDER_TARGETS; i++)
		{
			ct::RENDER_SURFACE_DESC surfaceDesc;
			if (mDesc.colorSurfaces[i].texture.isLoaded())
				surfaceDesc.texture = mDesc.colorSurfaces[i].texture->getCore();

			surfaceDesc.face = mDesc.colorSurfaces[i].face;
			surfaceDesc.numFaces = mDesc.colorSurfaces[i].numFaces;
			surfaceDesc.mipLevel = mDesc.colorSurfaces[i].mipLevel;

			coreDesc.colorSurfaces[i] = surfaceDesc;
		}

		if (mDesc.depthStencilSurface.texture.isLoaded())
			coreDesc.depthStencilSurface.texture = mDesc.depthStencilSurface.texture->getCore();

		coreDesc.depthStencilSurface.face = mDesc.depthStencilSurface.face;
		coreDesc.depthStencilSurface.numFaces = mDesc.depthStencilSurface.numFaces;
		coreDesc.depthStencilSurface.mipLevel = mDesc.depthStencilSurface.mipLevel;

		return ct::TextureManager::instance().createRenderTextureInternal(coreDesc);
	}

	CoreSyncData RenderTexture::syncToCore(FrameAlloc* allocator)
	{
		UINT32 size = sizeof(RenderTextureProperties);
		UINT8* buffer = allocator->alloc(size);

		RenderTextureProperties& props = const_cast<RenderTextureProperties&>(getProperties());

		memcpy(buffer, (void*)&props, size);
		return CoreSyncData(buffer, size);
	}

	const RenderTextureProperties& RenderTexture::getProperties() const
	{
		return static_cast<const RenderTextureProperties&>(getPropertiesInternal());
	}

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/

	RTTITypeBase* RenderTexture::getRTTIStatic()
	{
		return RenderTextureRTTI::instance();
	}

	RTTITypeBase* RenderTexture::getRTTI() const
	{
		return RenderTexture::getRTTIStatic();
	}

	namespace ct
	{
	RenderTexture::RenderTexture(const RENDER_TEXTURE_DESC& desc, UINT32 deviceIdx)
		:mDesc(desc)
	{ }

	void RenderTexture::initialize()
	{
		RenderTarget::initialize();

		for (UINT32 i = 0; i < BS_MAX_MULTIPLE_RENDER_TARGETS; i++)
		{
			if (mDesc.colorSurfaces[i].texture != nullptr)
			{
				SPtr<Texture> texture = mDesc.colorSurfaces[i].texture;

				if ((texture->getProperties().getUsage() & TU_RENDERTARGET) == 0)
					BS_EXCEPT(InvalidParametersException, "Provided texture is not created with render target usage.");

				mColorSurfaces[i] = texture->requestView(mDesc.colorSurfaces[i].mipLevel, 1,
					mDesc.colorSurfaces[i].face, mDesc.colorSurfaces[i].numFaces, GVU_RENDERTARGET);
			}
		}

		if (mDesc.depthStencilSurface.texture != nullptr)
		{
			SPtr<Texture> texture = mDesc.depthStencilSurface.texture;

			if ((texture->getProperties().getUsage() & TU_DEPTHSTENCIL) == 0)
				BS_EXCEPT(InvalidParametersException, "Provided texture is not created with depth stencil usage.");

			mDepthStencilSurface = texture->requestView(mDesc.depthStencilSurface.mipLevel, 1,
				mDesc.depthStencilSurface.face, mDesc.depthStencilSurface.numFaces, GVU_DEPTHSTENCIL);
		}

		throwIfBuffersDontMatch();
	}

	SPtr<RenderTexture> RenderTexture::create(const RENDER_TEXTURE_DESC& desc, UINT32 deviceIdx)
	{
		return TextureManager::instance().createRenderTexture(desc, deviceIdx);
	}

	void RenderTexture::syncToCore(const CoreSyncData& data)
	{
		RenderTextureProperties& props = const_cast<RenderTextureProperties&>(getProperties());
		props = data.getData<RenderTextureProperties>();
	}

	const RenderTextureProperties& RenderTexture::getProperties() const
	{
		return static_cast<const RenderTextureProperties&>(getPropertiesInternal());
	}

	void RenderTexture::throwIfBuffersDontMatch() const
	{
		UINT32 firstSurfaceIdx = (UINT32)-1;
		for (UINT32 i = 0; i < BS_MAX_MULTIPLE_RENDER_TARGETS; i++)
		{
			if (mColorSurfaces[i] == nullptr)
				continue;

			if (firstSurfaceIdx == (UINT32)-1)
			{
				firstSurfaceIdx = i;
				continue;
			}

			const TextureProperties& curTexProps = mDesc.colorSurfaces[i].texture->getProperties();
			const TextureProperties& firstTexProps = mDesc.colorSurfaces[firstSurfaceIdx].texture->getProperties();

			UINT32 curMsCount = curTexProps.getNumSamples();
			UINT32 firstMsCount = firstTexProps.getNumSamples();

			UINT32 curNumSlices = mColorSurfaces[i]->getNumArraySlices();
			UINT32 firstNumSlices = mColorSurfaces[firstSurfaceIdx]->getNumArraySlices();

			if (curMsCount == 0)
				curMsCount = 1;

			if (firstMsCount == 0)
				firstMsCount = 1;

			if (curTexProps.getWidth() != firstTexProps.getWidth() ||
				curTexProps.getHeight() != firstTexProps.getHeight() ||
				curTexProps.getDepth() != firstTexProps.getDepth() ||
				curMsCount != firstMsCount ||
				curNumSlices != firstNumSlices)
			{
				String errorInfo = "\nWidth: " + toString(curTexProps.getWidth()) + "/" + toString(firstTexProps.getWidth());
				errorInfo += "\nHeight: " + toString(curTexProps.getHeight()) + "/" + toString(firstTexProps.getHeight());
				errorInfo += "\nDepth: " + toString(curTexProps.getDepth()) + "/" + toString(firstTexProps.getDepth());
				errorInfo += "\nNum. slices: " + toString(curNumSlices) + "/" + toString(firstNumSlices);
				errorInfo += "\nMultisample Count: " + toString(curMsCount) + "/" + toString(firstMsCount);

				BS_EXCEPT(InvalidParametersException, "Provided color textures don't match!" + errorInfo);
			}
		}

		if (firstSurfaceIdx != (UINT32)-1)
		{
			const TextureProperties& firstTexProps = mDesc.colorSurfaces[firstSurfaceIdx].texture->getProperties();
			SPtr<TextureView> firstSurfaceView = mColorSurfaces[firstSurfaceIdx];

			UINT32 numSlices;
			if (firstTexProps.getTextureType() == TEX_TYPE_3D)
				numSlices = firstTexProps.getDepth();
			else
				numSlices = firstTexProps.getNumFaces();

			if ((firstSurfaceView->getFirstArraySlice() + firstSurfaceView->getNumArraySlices()) > numSlices)
			{
				BS_EXCEPT(InvalidParametersException, "Provided number of faces is out of range. Face: " +
					toString(firstSurfaceView->getFirstArraySlice() + firstSurfaceView->getNumArraySlices()) + ". Max num faces: " + toString(numSlices));
			}

			if (firstSurfaceView->getMostDetailedMip() > firstTexProps.getNumMipmaps())
			{
				BS_EXCEPT(InvalidParametersException, "Provided number of mip maps is out of range. Mip level: " +
					toString(firstSurfaceView->getMostDetailedMip()) + ". Max num mipmaps: " + toString(firstTexProps.getNumMipmaps()));
			}

			if (mDepthStencilSurface == nullptr)
				return;

			const TextureProperties& depthTexProps = mDesc.depthStencilSurface.texture->getProperties();
			UINT32 depthMsCount = depthTexProps.getNumSamples();
			UINT32 colorMsCount = firstTexProps.getNumSamples();

			if (depthMsCount == 0)
				depthMsCount = 1;

			if (colorMsCount == 0)
				colorMsCount = 1;

			if (depthTexProps.getWidth() != firstTexProps.getWidth() ||
				depthTexProps.getHeight() != firstTexProps.getHeight() ||
				depthMsCount != colorMsCount)
			{
				String errorInfo = "\nWidth: " + toString(depthTexProps.getWidth()) + "/" + toString(firstTexProps.getWidth());
				errorInfo += "\nHeight: " + toString(depthTexProps.getHeight()) + "/" + toString(firstTexProps.getHeight());
				errorInfo += "\nMultisample Count: " + toString(depthMsCount) + "/" + toString(colorMsCount);

				BS_EXCEPT(InvalidParametersException, "Provided texture and depth stencil buffer don't match!" + errorInfo);
			}
		}
	}
	}
}

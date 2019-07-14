//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsVulkanRenderTexture.h"
#include "BsVulkanFramebuffer.h"
#include "BsVulkanTexture.h"
#include "BsVulkanUtility.h"
#include "BsVulkanRenderAPI.h"
#include "BsVulkanDevice.h"
#include "BsVulkanRenderPass.h"

namespace bs
{
	VulkanRenderTexture::VulkanRenderTexture(const RENDER_TEXTURE_DESC& desc)
		:RenderTexture(desc), mProperties(desc, false)
	{

	}

	namespace ct
	{
	VulkanRenderTexture::VulkanRenderTexture(const RENDER_TEXTURE_DESC& desc, UINT32 deviceIdx)
		:RenderTexture(desc, deviceIdx), mProperties(desc, false), mDeviceIdx(deviceIdx), mFramebuffer(nullptr)
	{
		
	}

	VulkanRenderTexture::~VulkanRenderTexture()
	{
		mFramebuffer->destroy();
	}

	void VulkanRenderTexture::initialize()
	{
		RenderTexture::initialize();

		VULKAN_RENDER_PASS_DESC rpDesc;
		rpDesc.numSamples = mProperties.multisampleCount > 1 ? mProperties.multisampleCount : 1;
		rpDesc.offscreen = true;

		VULKAN_FRAMEBUFFER_DESC fbDesc;
		fbDesc.width = mProperties.width;
		fbDesc.height = mProperties.height;
		fbDesc.layers = mProperties.numSlices;

		for (UINT32 i = 0; i < BS_MAX_MULTIPLE_RENDER_TARGETS; ++i)
		{
			if (mColorSurfaces[i] == nullptr)
				continue;

			const SPtr<TextureView>& view = mColorSurfaces[i];
			VulkanTexture* texture = static_cast<VulkanTexture*>(mDesc.colorSurfaces[i].texture.get());

			VulkanImage* image = texture->getResource(mDeviceIdx);
			if (image == nullptr)
				continue;

			TextureSurface surface;
			surface.mipLevel = view->getMostDetailedMip();
			surface.numMipLevels = view->getNumMips();

			if (texture->getProperties().getTextureType() == TEX_TYPE_3D)
			{
				if(view->getFirstArraySlice() > 0)
					BS_LOG(Error, RenderBackend, "Non-zero array slice offset not supported when rendering to a 3D texture.");

				if (view->getNumArraySlices() > 1)
					BS_LOG(Error, RenderBackend, "Cannot specify array slices when rendering to a 3D texture.");

				surface.face = 0;
				surface.numFaces = mProperties.numSlices;

				fbDesc.color[i].baseLayer = 0;
			}
			else
			{
				surface.face = view->getFirstArraySlice();
				surface.numFaces = view->getNumArraySlices();

				fbDesc.color[i].baseLayer = view->getFirstArraySlice();
				fbDesc.layers = view->getNumArraySlices();
			}

			fbDesc.color[i].image = image;
			fbDesc.color[i].surface = surface;

			rpDesc.color[i].enabled = true;
			rpDesc.color[i].format = VulkanUtility::getPixelFormat(texture->getProperties().getFormat(),
																   texture->getProperties().isHardwareGammaEnabled());
		}

		if(mDepthStencilSurface != nullptr)
		{
			const SPtr<TextureView>& view = mDepthStencilSurface;
			VulkanTexture* texture = static_cast<VulkanTexture*>(mDesc.depthStencilSurface.texture.get());

			VulkanImage* image = texture->getResource(mDeviceIdx);
			if (image != nullptr)
			{
				TextureSurface surface;
				surface.mipLevel = view->getMostDetailedMip();
				surface.numMipLevels = view->getNumMips();

				if (texture->getProperties().getTextureType() == TEX_TYPE_3D)
				{
					if (view->getFirstArraySlice() > 0)
						BS_LOG(Error, RenderBackend, "Non-zero array slice offset not supported when rendering to a 3D texture.");

					if (view->getNumArraySlices() > 1)
						BS_LOG(Error, RenderBackend, "Cannot specify array slices when rendering to a 3D texture.");

					surface.face = 0;
					surface.numFaces = 1;
				}
				else
				{
					surface.face = view->getFirstArraySlice();
					surface.numFaces = view->getNumArraySlices();

					fbDesc.layers = view->getNumArraySlices();
				}

				fbDesc.depth.image = image;
				fbDesc.depth.surface = surface;
				fbDesc.depth.baseLayer = view->getFirstArraySlice();

				rpDesc.depth.enabled = true;
				rpDesc.depth.format = VulkanUtility::getPixelFormat(texture->getProperties().getFormat(),
																	texture->getProperties().isHardwareGammaEnabled());
			}
		}

		VulkanRenderAPI& rapi = static_cast<VulkanRenderAPI&>(RenderAPI::instance());
		SPtr<VulkanDevice> device = rapi._getDevice(mDeviceIdx);

		VulkanRenderPass* renderPass = VulkanRenderPasses::instance().get(device->getLogical(), rpDesc);
		mFramebuffer = device->getResourceManager().create<VulkanFramebuffer>(renderPass, fbDesc);
	}

	void VulkanRenderTexture::getCustomAttribute(const String& name, void* data) const
	{
		if (name == "FB")
		{
			VulkanFramebuffer** fb = (VulkanFramebuffer**)data;
			*fb = mFramebuffer;
			return;
		}
	}		
	}
}

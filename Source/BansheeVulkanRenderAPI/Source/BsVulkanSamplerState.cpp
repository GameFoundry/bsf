//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsVulkanSamplerState.h"
#include "BsVulkanDevice.h"
#include "BsVulkanUtility.h"

namespace BansheeEngine
{
	VulkanSampler::VulkanSampler(VulkanResourceManager* owner, VkSampler sampler)
		:VulkanResource(owner, true), mSampler(sampler)
	{ }

	VulkanSampler::~VulkanSampler()
	{
		vkDestroySampler(mOwner->getDevice().getLogical(), mSampler, gVulkanAllocator);
	}

	VulkanSamplerStateCore::VulkanSamplerStateCore(const SAMPLER_STATE_DESC& desc)
		:SamplerStateCore(desc), mSampler(nullptr)
	{ }

	VulkanSamplerStateCore::~VulkanSamplerStateCore()
	{
		if(mSampler != nullptr)
			mSampler->destroy();
	}

	void VulkanSamplerStateCore::createInternal()
	{
		FilterOptions minFilter = getProperties().getTextureFiltering(FT_MIN);
		FilterOptions magFilter = getProperties().getTextureFiltering(FT_MAG);
		FilterOptions mipFilter = getProperties().getTextureFiltering(FT_MIP);

		bool anisotropy = minFilter == FO_ANISOTROPIC || magFilter == FO_ANISOTROPIC || mipFilter == FO_ANISOTROPIC;
		const UVWAddressingMode& addressMode = getProperties().getTextureAddressingMode();

		CompareFunction compareFunc = getProperties().getComparisonFunction();

		VkSamplerCreateInfo samplerInfo;
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.flags = 0;
		samplerInfo.pNext = nullptr;
		samplerInfo.magFilter = VulkanUtility::getFilter(magFilter);
		samplerInfo.minFilter = VulkanUtility::getFilter(minFilter);
		samplerInfo.mipmapMode = VulkanUtility::getMipFilter(mipFilter);
		samplerInfo.addressModeU = VulkanUtility::getAddressingMode(addressMode.u);
		samplerInfo.addressModeV = VulkanUtility::getAddressingMode(addressMode.v);
		samplerInfo.addressModeW = VulkanUtility::getAddressingMode(addressMode.w);
		samplerInfo.mipLodBias = getProperties().getTextureMipmapBias();
		samplerInfo.anisotropyEnable = anisotropy;
		samplerInfo.maxAnisotropy = (float)getProperties().getTextureAnisotropy();
		samplerInfo.compareEnable = compareFunc != CMPF_ALWAYS_PASS;
		samplerInfo.compareOp = VulkanUtility::getCompareOp(compareFunc);
		samplerInfo.minLod = mProperties.getMinimumMip();
		samplerInfo.maxLod = mProperties.getMaximumMip();
		samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		samplerInfo.unnormalizedCoordinates = false;

		// TODO - Create state per device

		VkSampler sampler;
		VkResult result = vkCreateSampler(mOwner->getDevice().getLogical(), &samplerInfo, gVulkanAllocator, &sampler);
		assert(result == VK_SUCCESS);
	}
}
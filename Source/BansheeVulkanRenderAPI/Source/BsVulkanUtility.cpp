//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsVulkanUtility.h"
#include "BsException.h"

namespace BansheeEngine
{
	VkFormat VulkanUtility::getPixelFormat(PixelFormat format, bool sRGB)
	{
		switch (format)
		{
		case PF_R8:
			if(sRGB)
				return VK_FORMAT_R8_SRGB;

			return VK_FORMAT_R8_UNORM;
		case PF_R8G8:
			if (sRGB)
				return VK_FORMAT_R8G8_SRGB;

			return VK_FORMAT_R8G8_UNORM;
		case PF_R8G8B8:
			if (sRGB)
				return VK_FORMAT_R8G8B8_SRGB;

			return VK_FORMAT_R8G8B8_UNORM;
		case PF_R8G8B8A8:
			if (sRGB)
				return VK_FORMAT_R8G8B8A8_SRGB;

			return VK_FORMAT_R8G8B8A8_UNORM;
		case PF_B8G8R8A8:
		case PF_B8G8R8X8:
			if (sRGB)
				return VK_FORMAT_B8G8R8A8_SRGB;

			return VK_FORMAT_B8G8R8A8_UNORM;
		case PF_FLOAT16_R:
			return VK_FORMAT_R16_SFLOAT;
		case PF_FLOAT16_RG:
			return VK_FORMAT_R16G16_SFLOAT;
		case PF_FLOAT16_RGB:
			return VK_FORMAT_R16G16B16_SFLOAT;
		case PF_FLOAT16_RGBA:
			return VK_FORMAT_R16G16B16A16_SFLOAT;
		case PF_FLOAT32_R:
			return VK_FORMAT_R32_SFLOAT;
		case PF_FLOAT32_RG:
			return VK_FORMAT_R32G32_SFLOAT;
		case PF_FLOAT32_RGB:
			return VK_FORMAT_R32G32B32_SFLOAT;
		case PF_FLOAT32_RGBA:
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		case PF_BC1:
			if (sRGB)
				return VK_FORMAT_BC1_RGB_SRGB_BLOCK;

			return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
		case PF_BC1a:
			if (sRGB)
				return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;

			return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
		case PF_BC2:
			if (sRGB)
				return VK_FORMAT_BC2_SRGB_BLOCK;

			return VK_FORMAT_BC2_UNORM_BLOCK;
		case PF_BC3:
			if (sRGB)
				return VK_FORMAT_BC3_SRGB_BLOCK;

			return VK_FORMAT_BC3_UNORM_BLOCK;
		case PF_BC4:
			return VK_FORMAT_BC4_SNORM_BLOCK;
		case PF_BC5:
			return VK_FORMAT_BC5_UNORM_BLOCK;
		case PF_BC6H:
			return VK_FORMAT_BC6H_SFLOAT_BLOCK;
		case PF_BC7:
			if (sRGB)
				return VK_FORMAT_BC7_SRGB_BLOCK;

			return VK_FORMAT_BC7_UNORM_BLOCK;
		case PF_D32_S8X24:
			return VK_FORMAT_D32_SFLOAT_S8_UINT;
		case PF_D24S8:
			return VK_FORMAT_D24_UNORM_S8_UINT;
		case PF_D32:
			return VK_FORMAT_D32_SFLOAT;
		case PF_D16:
			return VK_FORMAT_D16_UNORM;
		case PF_FLOAT_R11G11B10:
			return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
		case PF_UNORM_R10G10B10A2:
			return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
		case PF_A8R8G8B8:
		case PF_A8B8G8R8:
		case PF_X8R8G8B8:
		case PF_X8B8G8R8:
		case PF_UNKNOWN:
		default:
			return VK_FORMAT_UNDEFINED;
		}
	}

	VkSampleCountFlagBits VulkanUtility::getSampleFlags(UINT32 numSamples)
	{
		switch(numSamples)
		{
		case 1:
			return VK_SAMPLE_COUNT_1_BIT;
		case 2:
			return VK_SAMPLE_COUNT_2_BIT;
		case 4:
			return VK_SAMPLE_COUNT_4_BIT;
		case 8:
			return VK_SAMPLE_COUNT_8_BIT;
		case 16:
			return VK_SAMPLE_COUNT_16_BIT;
		case 32:
			return VK_SAMPLE_COUNT_32_BIT;
		case 64:
			return VK_SAMPLE_COUNT_64_BIT;
		}

		BS_EXCEPT(RenderingAPIException, "Unsupported sample count: " + toString(numSamples));
		return VK_SAMPLE_COUNT_1_BIT;
	}
}

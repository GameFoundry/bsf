//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsVulkanRenderAPIFactory.h"
#include "BsRenderAPI.h"

namespace bs { namespace ct
{
	const char* SystemName = "BansheeVulkanRenderAPI";

	void VulkanRenderAPIFactory::create()
	{
		RenderAPI::startUp<VulkanRenderAPI>();
	}

	VulkanRenderAPIFactory::InitOnStart VulkanRenderAPIFactory::initOnStart;
}}
//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsVulkanPrerequisites.h"
#include "BsVulkanResource.h"
#include "BsQueryManager.h"

namespace BansheeEngine
{
	/** @addtogroup Vulkan
	 *  @{
	 */

	/**	Handles creation of Vulkan queries. */
	class VulkanQueryManager : public QueryManager
	{
	public:
		VulkanQueryManager(VulkanRenderAPI& rapi);
		~VulkanQueryManager();

		/** @copydoc QueryManager::createEventQuery */
		SPtr<EventQuery> createEventQuery(UINT32 deviceIdx = 0) const override;

		/** @copydoc QueryManager::createTimerQuery */
		SPtr<TimerQuery> createTimerQuery(UINT32 deviceIdx = 0) const override;

		/** @copydoc QueryManager::createOcclusionQuery */
		SPtr<OcclusionQuery> createOcclusionQuery(bool binary, UINT32 deviceIdx = 0) const override;

	private:
		VulkanRenderAPI& mRenderAPI;
	};

	/** Wrapper around a single query in a Vulkan query pool object. */
	class VulkanQuery : public VulkanResource
	{
	public:
		VulkanQuery(VulkanResourceManager* owner);
		~VulkanQuery();

		/** Returns the internal handle to the Vulkan object. */
		bool setUsed(bool used) const { return mEvent; }

	private:
		VkEvent mEvent;
	};


	/** @} */
}
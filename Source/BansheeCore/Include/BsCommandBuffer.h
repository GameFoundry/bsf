//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/** @addtogroup RenderAPI
	 *  @{
	 */

	/** 
	 * Contains a list of render API commands that can be queued for execution on the GPU. User is allowed to populate the
	 * command buffer from any thread, ensuring render API command generation can be multi-threaded. Command buffers
	 * must always be created on the core thread. Same command buffer cannot be used on multiple threads simulateously
	 * without external synchronization.
	 */
	class BS_CORE_EXPORT CommandBuffer
	{
	public:
		/** @copydoc CommandBufferManager::create */
		static SPtr<CommandBuffer> create(CommandBufferType type, UINT32 deviceIdx = 0, UINT32 syncMask = 0xFFFFFFFF, 
			bool secondary = false);

	protected:
		CommandBuffer(CommandBufferType type, UINT32 syncMask, bool secondary);

		CommandBufferType mType;
		UINT32 mSyncMask;
		bool mIsSecondary;
	};

	/** @} */

	/** @addtogroup RenderAPI-Internal
	 *  @{
	 */

	/**
	 * Command buffer implementation for render API's that do not support multi-threaded command generation. Instead all
	 * commands are stored in an internal buffer, and then sent to the actual render API when the buffer is executed.
	 */
	class BS_CORE_EXPORT SimpleCommandBuffer : CommandBuffer
	{
	public:
		/** Registers a new command in the command buffer. */
		void queueCommand(const std::function<void()> command);

		/** Appends all commands from the secondary buffer into this command buffer. */
		void appendSecondary(const SPtr<SimpleCommandBuffer>& secondaryBuffer);

		/** Executes all commands in the command buffer. Not supported on secondary buffer. */
		void executeCommands();

		/** Removes all commands from the command buffer. */
		void clear();

	private:
		friend class CommandBufferManager;

		SimpleCommandBuffer(CommandBufferType type, UINT32 deviceIdx, UINT32 syncMask, bool secondary);

		UINT32 mDeviceIdx;
		Vector<std::function<void()>> mCommands;
	};

	/** @} */
}
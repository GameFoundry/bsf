//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsCommandBuffer.h"
#include "BsCommandBufferManager.h"

namespace BansheeEngine
{
	CommandBuffer::CommandBuffer(CommandBufferType type, UINT32 syncMask, bool secondary)
		:mType(type), mSyncMask(syncMask), mIsSecondary(secondary)
	{

	}

	SPtr<CommandBuffer> CommandBuffer::create(CommandBufferType type, UINT32 deviceIdx, UINT32 syncMask,
		bool secondary)
	{
		return CommandBufferManager::instance().create(type, deviceIdx, syncMask, secondary);
	}

	SimpleCommandBuffer::SimpleCommandBuffer(CommandBufferType type, UINT32 deviceIdx, UINT32 syncMask, bool secondary)
		: CommandBuffer(type, syncMask, secondary), mDeviceIdx(deviceIdx)
	{

	}

	void SimpleCommandBuffer::queueCommand(const std::function<void()> command)
	{
		mCommands.push_back(command);
	}

	void SimpleCommandBuffer::appendSecondary(const SPtr<SimpleCommandBuffer>& secondaryBuffer)
	{
#if BS_DEBUG_MODE
		if(!secondaryBuffer->mIsSecondary)
		{
			LOGERR("Cannot append a command buffer that is not secondary.");
			return;
		}

		if(mIsSecondary)
		{
			LOGERR("Cannot append a buffer to a secondary command buffer.");
			return;
		}
#endif

		for (auto& entry : secondaryBuffer->mCommands)
			mCommands.push_back(entry);
	}

	void SimpleCommandBuffer::executeCommands()
	{
#if BS_DEBUG_MODE
		if (mIsSecondary)
		{
			LOGERR("Cannot execute commands on a secondary buffer.");
			return;
		}
#endif

		for (auto& entry : mCommands)
			entry();
	}

	void SimpleCommandBuffer::clear()
	{
		mCommands.clear();
	}
}
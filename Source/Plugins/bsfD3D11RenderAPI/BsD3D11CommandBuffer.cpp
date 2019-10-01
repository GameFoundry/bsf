//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsD3D11CommandBuffer.h"
#include "BsD3D11Device.h"
#include "BsD3D11EventQuery.h"

namespace bs { namespace ct
{
	D3D11CommandBuffer::D3D11CommandBuffer(GpuQueueType type, UINT32 deviceIdx, UINT32 queueIdx, bool secondary)
		: CommandBuffer(type, deviceIdx, queueIdx, secondary)
	{
		if (deviceIdx != 0)
			BS_EXCEPT(InvalidParametersException, "Only a single device supported on DX11.");
	}

	void D3D11CommandBuffer::queueCommand(const std::function<void()> command)
	{
#if BS_DEBUG_MODE
		if (getState() == CommandBufferState::Executing)
		{
			BS_LOG(Error, RenderBackend, "Cannot modify a command buffer that's still executing.");
			return;
		}
#endif
		
		mCommands.push_back(command);
	}

	void D3D11CommandBuffer::appendSecondary(const SPtr<D3D11CommandBuffer>& secondaryBuffer)
	{
#if BS_DEBUG_MODE
		if (!secondaryBuffer->mIsSecondary)
		{
			BS_LOG(Error, RenderBackend, "Cannot append a command buffer that is not secondary.");
			return;
		}

		if (mIsSecondary)
		{
			BS_LOG(Error, RenderBackend, "Cannot append a buffer to a secondary command buffer.");
			return;
		}
#endif

		for (auto& entry : secondaryBuffer->mCommands)
			mCommands.push_back(entry);
	}

	void D3D11CommandBuffer::executeCommands()
	{
#if BS_DEBUG_MODE
		if (mIsSecondary)
		{
			BS_LOG(Error, RenderBackend, "Cannot execute commands on a secondary buffer.");
			return;
		}

		if(getState() == CommandBufferState::Executing)
		{
			BS_LOG(Error, RenderBackend, "Cannot submit a command buffer that's still executing.");
			return;
		}
#endif
		
		for (auto& entry : mCommands)
			entry();

		mFence = bs_shared_ptr_new<D3D11EventQuery>(mDeviceIdx);
		mFence->begin();
		mIsSubmitted = true;
	}

	CommandBufferState D3D11CommandBuffer::getState() const
	{
		if (mIsSubmitted)
			return isComplete() ? CommandBufferState::Done : CommandBufferState::Executing;

		return mCommands.empty() ? CommandBufferState::Empty : CommandBufferState::Recording;
	}

	void D3D11CommandBuffer::reset()
	{
		mCommands.clear();
		mIsSubmitted = false;
	}

	bool D3D11CommandBuffer::isComplete() const
	{
		return mFence && mFence->isReady();
	}
}}

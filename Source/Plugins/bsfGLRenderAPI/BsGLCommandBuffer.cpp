//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsGLCommandBuffer.h"

namespace bs { namespace ct
{
	GLCommandBuffer::GLCommandBuffer(GpuQueueType type, UINT32 deviceIdx, UINT32 queueIdx, bool secondary)
		: CommandBuffer(type, deviceIdx, queueIdx, secondary)
	{
		if (deviceIdx != 0)
			BS_EXCEPT(InvalidParametersException, "Only a single device supported on OpenGL.");
	}

	void GLCommandBuffer::queueCommand(const std::function<void()> command)
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

	void GLCommandBuffer::appendSecondary(const SPtr<GLCommandBuffer>& secondaryBuffer)
	{
#if BS_DEBUG_MODE
		if(!secondaryBuffer->mIsSecondary)
		{
			BS_LOG(Error, RenderBackend, "Cannot append a command buffer that is not secondary.");
			return;
		}

		if(mIsSecondary)
		{
			BS_LOG(Error, RenderBackend, "Cannot append a buffer to a secondary command buffer.");
			return;
		}
#endif

		for (auto& entry : secondaryBuffer->mCommands)
			mCommands.push_back(entry);
	}

	void GLCommandBuffer::executeCommands()
	{
#if BS_DEBUG_MODE
		if (mIsSecondary)
		{
			BS_LOG(Error, RenderBackend, "Cannot execute commands on a secondary buffer.");
			return;
		}

		if (getState() == CommandBufferState::Executing)
		{
			BS_LOG(Error, RenderBackend, "Cannot submit a command buffer that's still executing.");
			return;
		}
#endif

		for (auto& entry : mCommands)
			entry();

		mCommands.clear();

		if (glIsSync(mFence))
			glDeleteSync(mFence);

		glFlush();
		mFence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	}

	CommandBufferState GLCommandBuffer::getState() const
	{
		if (mIsSubmitted)
			return isComplete() ? CommandBufferState::Done : CommandBufferState::Executing;

		return mCommands.empty() ? CommandBufferState::Empty : CommandBufferState::Recording;
	}

	void GLCommandBuffer::reset()
	{
		mCommands.clear();
		mIsSubmitted = false;
	}

	bool GLCommandBuffer::isComplete() const
	{
		GLenum result = glClientWaitSync(mFence, 0, 0);
		return result == GL_ALREADY_SIGNALED || GL_CONDITION_SATISFIED;
	}

}}

#pragma once

#include "CmPrerequisites.h"
#include "CmRenderOperation.h"
#include "CmRenderCommand.h"

namespace CamelotEngine
{
	/**
	 * @brief	Used for multithreaded rendering. One render command buffer should exist per thread.
	 * 			All render operations are recorded in the buffer, and then played back
	 * 			on the render thread. 
	 */
	class RenderCommandBuffer
	{
	public:
		RenderCommandBuffer(CM_THREAD_ID_TYPE threadId);
		~RenderCommandBuffer();

		/**
		 * @brief	Adds a new render command that will be executed when the playback is initiated.
		 * 			CommandBuffer takes ownership of the command, and will release it after playback is complete.
		 */
		void render(RenderOperation renderOp);

		/**
		 * @brief	Adds a new apply pass command that will be executed when the playback is initiated. Selected
		 * 			pass will be used for all subsequent render commands.
		 */
		void applyPass(PassPtr pass, PassParametersPtr passParameters);

	private:
		CM_MUTEX(mCommandBufferMutex)
		vector<RenderCommand>::type* mRenderCommands;

		PassPtr mActivePass;
		PassParametersPtr mActiveParameters;

		CM_THREAD_ID_TYPE mMyThreadId;

		void throwIfInvalidThread();

	private:
		friend class RenderSystem;

		/**
		 * @brief	Executed all commands stored in the buffers, in the order in which they were added.
		 * 			Must be called on the render thread.
		 */
		void playDeferredCommands();
	};
}
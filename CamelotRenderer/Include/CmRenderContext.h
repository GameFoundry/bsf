#pragma once

#include "CmPrerequisites.h"
#include "CmRenderOperation.h"
#include "CmRenderCommand.h"

namespace CamelotEngine
{
	class RenderContext
	{
	public:
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

		/**
		 * @brief	Executed all commands stored in the buffers, in the order in which they were added.
		 * 			Must be called on the render thread.
		 */
		void playDeferredCommands();

	private:
		vector<RenderCommand>::type mRenderCommands;

		PassPtr mActivePass;
		PassParametersPtr mActiveParameters;
	};
}
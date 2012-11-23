#pragma once

#include "CmPrerequisites.h"

namespace CamelotEngine
{
	class DeferredRenderSystem
	{
	public:
		/**
		 * @brief	Wrapper around RenderSystem method of the same name. See RenderSystem doc.
		 * 			
		 * @see		RenderSystem::render()
		 */
		void render(const RenderOperation& op);

		void submitToGpu();

	private:
		vector<DeferredGpuCommand*>::type mRenderCommands;
	};
}
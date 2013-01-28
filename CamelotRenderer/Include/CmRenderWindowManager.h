#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"
#include "CmRenderWindow.h"

namespace CamelotEngine
{
	class CM_EXPORT RenderWindowManager : public Module<RenderWindowManager>
	{
	public:
		/** Creates a new rendering window.
		*/
		RenderWindowPtr create(const RENDER_WINDOW_DESC& desc);

	protected:
		virtual RenderWindowPtr createImpl(const RENDER_WINDOW_DESC& desc) = 0;
	};
}
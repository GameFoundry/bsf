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
		RenderWindowPtr create(RENDER_WINDOW_DESC& desc, RenderWindowPtr parentWindow);

	protected:
		virtual RenderWindowPtr createImpl(RENDER_WINDOW_DESC& desc, RenderWindowPtr parentWindow) = 0;
	};
}
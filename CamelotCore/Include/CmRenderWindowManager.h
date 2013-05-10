#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"
#include "CmRenderWindow.h"

namespace CamelotFramework
{
	class CM_EXPORT RenderWindowManager : public Module<RenderWindowManager>
	{
	public:
		/** Creates a new rendering window.
		*/
		RenderWindowPtr create(RENDER_WINDOW_DESC& desc, RenderWindowPtr parentWindow);

		std::vector<RenderWindow*> getRenderWindows() const;
	protected:
		friend class RenderWindow;

		CM_MUTEX(mWindowMutex);
		std::vector<RenderWindow*> mCreatedWindows;

		virtual RenderWindowPtr createImpl(RENDER_WINDOW_DESC& desc, RenderWindowPtr parentWindow) = 0;

		void windowDestroyed(RenderWindow* window);
	};
}
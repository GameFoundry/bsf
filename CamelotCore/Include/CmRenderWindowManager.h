#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"
#include "CmRenderWindow.h"
#include <boost/signals.hpp>

namespace CamelotFramework
{
	class CM_EXPORT RenderWindowManager : public Module<RenderWindowManager>
	{
	public:
		RenderWindowManager();

		/**
		 * @brief	Creates a new rendering window.
		 */
		RenderWindowPtr create(RENDER_WINDOW_DESC& desc, RenderWindowPtr parentWindow);

		/**
		 * @brief	Called once per frame. Dispatches events. Internal method.
		 */
		void _update();

		Vector<RenderWindow*>::type getRenderWindows() const;

		boost::signal<void(RenderWindow&)> onFocusGained;
		boost::signal<void(RenderWindow&)> onFocusLost;
		boost::signal<void(RenderWindow&)> onMovedOrResized;
	protected:
		friend class RenderWindow;

		CM_MUTEX(mWindowMutex);
		Vector<RenderWindow*>::type mCreatedWindows;

		RenderWindow* mWindowInFocus;
		RenderWindow* mNewWindowInFocus;
		Vector<RenderWindow*>::type mMovedOrResizedWindows;

		virtual RenderWindowPtr createImpl(RENDER_WINDOW_DESC& desc, RenderWindowPtr parentWindow) = 0;

		void windowDestroyed(RenderWindow* window);
		void windowFocusReceived(RenderWindow* window);
		void windowMovedOrResized(RenderWindow* window);
	};
}
#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"
#include "CmRenderWindow.h"
#include "BsEvent.h"

namespace BansheeEngine
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
		void update();

		Vector<RenderWindow*> getRenderWindows() const;

		Event<void(RenderWindow&)> onFocusGained;
		Event<void(RenderWindow&)> onFocusLost;
	protected:
		friend class RenderWindow;

		CM_MUTEX(mWindowMutex);
		Vector<RenderWindow*> mCreatedWindows;

		RenderWindow* mWindowInFocus;
		RenderWindow* mNewWindowInFocus;
		Vector<RenderWindow*> mMovedOrResizedWindows;

		virtual RenderWindowPtr createImpl(RENDER_WINDOW_DESC& desc, RenderWindowPtr parentWindow) = 0;

		void windowDestroyed(RenderWindow* window);
		void windowFocusReceived(RenderWindow* window);
		void windowFocusLost(RenderWindow* window);
		void windowMovedOrResized(RenderWindow* window);
	};
}
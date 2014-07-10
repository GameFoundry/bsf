//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"
#include "BsRenderWindow.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	/**
	 * @brief	Handles creation and internal updates relating to render windows.
	 *
	 * @note	Sim thread.
	 */
	class BS_CORE_EXPORT RenderWindowManager : public Module<RenderWindowManager>
	{
	public:
		RenderWindowManager();

		/**
		* @brief	Creates a new render window using the specified options. Optionally
		*			makes the created window a child of another window.
		*/
		RenderWindowPtr create(RENDER_WINDOW_DESC& desc, RenderWindowPtr parentWindow);

		/**
		 * @brief	Called once per frame. Dispatches events.
		 * 
		 * @note	Internal method.
		 */
		void _update();

		/**
		 * @brief	Returns a list of all open render windows.
		 */
		Vector<RenderWindow*> getRenderWindows() const;

		/**
		 * @brief	Event that is triggered when a window gains focus.
		 */
		Event<void(RenderWindow&)> onFocusGained;

		/**
		 * @brief	Event that is triggered when a window loses focus.
		 */
		Event<void(RenderWindow&)> onFocusLost;
	protected:
		friend class RenderWindow;

		/**
		 * @copydoc	create
		 */
		virtual RenderWindowPtr createImpl(RENDER_WINDOW_DESC& desc, RenderWindowPtr parentWindow) = 0;

		/**
		 * @brief	Called by the core thread when window is destroyed.
		 */
		void windowDestroyed(RenderWindow* window);

		/**
		 * @brief	Called by the core thread when window receives focus.
		 */
		void windowFocusReceived(RenderWindow* window);

		/**
		 * @brief	Called by the core thread when window loses focus.
		 */
		void windowFocusLost(RenderWindow* window);

		/**
		 * @brief	Called by the core thread when window is moved or resized.
		 */
		void windowMovedOrResized(RenderWindow* window);

	protected:
		BS_MUTEX(mWindowMutex);
		Vector<RenderWindow*> mCreatedWindows;

		RenderWindow* mWindowInFocus;
		RenderWindow* mNewWindowInFocus;
		Vector<RenderWindow*> mMovedOrResizedWindows;
	};
}
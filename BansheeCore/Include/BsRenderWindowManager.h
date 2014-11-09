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
	 * @note	Sim thread only.
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

		/**
		 * @brief	Event that is triggered when mouse leaves a window.
		 */
		Event<void(RenderWindow&)> onMouseLeftWindow;
	protected:
		friend class RenderWindow;

		/**
		 * @brief	Called by the core thread when window is destroyed.
		 */
		void windowDestroyed(RenderWindow* window);

		/**
		 * @brief	Called by the core thread when window receives focus.
		 */
		void windowFocusReceived(RenderWindowCore* window);

		/**
		 * @brief	Called by the core thread when window loses focus.
		 */
		void windowFocusLost(RenderWindowCore* window);

		/**
		 * @brief	Called by the core thread when window is moved or resized.
		 */
		void windowMovedOrResized(RenderWindowCore* window);

		/**
		 * @brief	Called by the core thread when mouse leaves a window.
		 */
		void windowMouseLeft(RenderWindowCore* window);

		/**
		 * @brief	Finds a sim thread equivalent of the provided core thread window implementation.
		 */
		RenderWindow* getNonCore(const RenderWindowCore* window) const;

		/**
		 * @copydoc	create
		 */
		virtual RenderWindowPtr createImpl(RENDER_WINDOW_DESC& desc, const RenderWindowPtr& parentWindow) = 0;

	protected:
		BS_MUTEX(mWindowMutex);
		Vector<RenderWindow*> mCreatedWindows;
		Map<const RenderWindowCore*, RenderWindow*> mCoreToNonCoreMap;

		RenderWindow* mWindowInFocus;
		RenderWindow* mNewWindowInFocus;
		Vector<RenderWindow*> mMovedOrResizedWindows;
		Vector<RenderWindow*> mMouseLeftWindows;
	};

	/**
	 * @brief	Handles creation and internal updates relating to render windows.
	 *
	 * @note	Core thread only.
	 */
	class BS_CORE_EXPORT RenderWindowCoreManager : public Module<RenderWindowCoreManager>
	{
	public:
		/**
		 * @copydoc	RenderWindowCoreManager::create
		 */
		SPtr<RenderWindowCore> create(RENDER_WINDOW_DESC& desc);

		/**
		 * @brief	Returns a list of all open render windows.
		 */
		Vector<RenderWindowCore*> getRenderWindows() const;

	protected:
		friend class RenderWindowCore;
		friend class RenderWindow;

		/**
		 * @copydoc	create
		 */
		virtual SPtr<RenderWindowCore> createInternal(RENDER_WINDOW_DESC& desc) = 0;

		/**
		 * @brief	Called whenever a window is created.
		 */
		void windowCreated(RenderWindowCore* window);

		/**
		 * @brief	Called by the core thread when window is destroyed.
		 */
		void windowDestroyed(RenderWindowCore* window);

		BS_MUTEX(mWindowMutex);
		Vector<RenderWindowCore*> mCreatedWindows;
	};
}
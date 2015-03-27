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
	 * @note	Internal class.
	 */
	class BS_CORE_EXPORT RenderWindowManager : public Module<RenderWindowManager>
	{
		/**
		 * @brief	Holds information about a window that was moved or resized.
		 */
		struct MoveOrResizeData
		{
			INT32 x, y;
			UINT32 width, height;
			RenderWindow* window;
		};

	public:
		RenderWindowManager();
		~RenderWindowManager();

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
		 * @brief	Called by the core thread when window is destroyed.
		 */
		void notifyWindowDestroyed(RenderWindow* window);

		/**
		 * @brief	Called by the core thread when window receives focus.
		 */
		void notifyFocusReceived(RenderWindowCore* window);

		/**
		 * @brief	Called by the core thread when window loses focus.
		 */
		void notifyFocusLost(RenderWindowCore* window);

		/**
		 * @brief	Called by the core thread when window is moved or resized.
		 */
		void notifyMovedOrResized(RenderWindowCore* window);

		/**
		 * @brief	Called by the sim thread when window properties change.
		 */
		void notifySyncDataDirty(RenderWindowCore* coreWindow);

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
		Vector<MoveOrResizeData> mMovedOrResizedWindows;
		Vector<RenderWindow*> mMouseLeftWindows;
		UnorderedSet<RenderWindow*> mDirtyProperties;
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
		 * @brief	Called once per frame. Dispatches events.
		 * 
		 * @note	Internal method.
		 */
		void _update();

		/**
		 * @brief	Called by the core thread when window properties change.
		 */
		void notifySyncDataDirty(RenderWindowCore* window);

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
		UnorderedSet<RenderWindowCore*> mDirtyProperties;
	};
}
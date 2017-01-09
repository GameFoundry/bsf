//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"
#include "BsRenderWindow.h"
#include "BsEvent.h"

namespace bs
{
	/** @addtogroup RenderAPI-Internal
	 *  @{
	 */

	/** Handles creation and internal updates relating to render windows. */
	class BS_CORE_EXPORT RenderWindowManager : public Module<RenderWindowManager>
	{
		/**	Holds information about a window that was moved or resized. */
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
		 * Creates a new render window using the specified options. Optionally makes the created window a child of another
		 * window.
		 */
		SPtr<RenderWindow> create(RENDER_WINDOW_DESC& desc, SPtr<RenderWindow> parentWindow);

		/** Called once per frame. Dispatches events. */
		void _update();

		/** Called by the core thread when window is destroyed. */
		void notifyWindowDestroyed(RenderWindow* window);

		/**	Called by the core thread when window receives focus. */
		void notifyFocusReceived(ct::RenderWindow* window);

		/**	Called by the core thread when window loses focus. */
		void notifyFocusLost(ct::RenderWindow* window);

		/**	Called by the core thread when window is moved or resized. */
		void notifyMovedOrResized(ct::RenderWindow* window);

		/**	Called by the sim thread when window properties change. */
		void notifySyncDataDirty(ct::RenderWindow* coreWindow);

		/**	Returns a list of all open render windows. */
		Vector<RenderWindow*> getRenderWindows() const;

		/** Event that is triggered when a window gains focus. */
		Event<void(RenderWindow&)> onFocusGained;

		/**	Event that is triggered when a window loses focus. */
		Event<void(RenderWindow&)> onFocusLost;

		/**	Event that is triggered when mouse leaves a window. */
		Event<void(RenderWindow&)> onMouseLeftWindow;
	protected:
		friend class RenderWindow;

		/**	Called by the core thread when mouse leaves a window. */
		void windowMouseLeft(ct::RenderWindow* window);

		/**	Finds a sim thread equivalent of the provided core thread window implementation. */
		RenderWindow* getNonCore(const ct::RenderWindow* window) const;

		/** @copydoc create */
		virtual SPtr<RenderWindow> createImpl(RENDER_WINDOW_DESC& desc, UINT32 windowId, const SPtr<RenderWindow>& parentWindow) = 0;

	protected:
		mutable Mutex mWindowMutex;
		Map<UINT32, RenderWindow*> mWindows;

		RenderWindow* mWindowInFocus;
		RenderWindow* mNewWindowInFocus;
		Vector<MoveOrResizeData> mMovedOrResizedWindows;
		Vector<RenderWindow*> mMouseLeftWindows;
		UnorderedSet<RenderWindow*> mDirtyProperties;
	};

	namespace ct
	{
	/**
	 * Handles creation and internal updates relating to render windows.
	 *
	 * @note	Core thread only.
	 */
	class BS_CORE_EXPORT RenderWindowManager : public Module<RenderWindowManager>
	{
	public:
		RenderWindowManager();

		/** Creates a new render window using the specified options. */
		SPtr<RenderWindow> create(RENDER_WINDOW_DESC& desc);

		/** Called once per frame. Dispatches events. */
		void _update();

		/**	Called by the core thread when window properties change. */
		void notifySyncDataDirty(RenderWindow* window);

		/**	Returns a list of all open render windows. */
		Vector<RenderWindow*> getRenderWindows() const;

	protected:
		friend class RenderWindow;
		friend class bs::RenderWindow;
		friend class bs::RenderWindowManager;

		/** @copydoc create */
		virtual SPtr<RenderWindow> createInternal(RENDER_WINDOW_DESC& desc, UINT32 windowId) = 0;

		/**	Called whenever a window is created. */
		void windowCreated(RenderWindow* window);

		/**	Called by the core thread when window is destroyed. */
		void windowDestroyed(RenderWindow* window);

		mutable Mutex mWindowMutex;
		Vector<RenderWindow*> mCreatedWindows;
		UnorderedSet<RenderWindow*> mDirtyProperties;
		std::atomic_uint mNextWindowId;
	};
	}

	/** @} */
}
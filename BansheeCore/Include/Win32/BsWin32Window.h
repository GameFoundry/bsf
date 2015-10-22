#pragma once

#include "BsCorePrerequisites.h"
#include "BsVector2I.h"
#include "windows.h"

namespace BansheeEngine
{
	/**
	 * @brief	Descriptor used for creating a platform specific native window.
	 */
	struct BS_CORE_EXPORT WINDOW_DESC
	{
		WINDOW_DESC()
			: module(nullptr), monitor(nullptr), parent(nullptr), external(nullptr), width(0), height(0), fullscreen(false)
			, hidden(false), left(-1), top(-1), title(""), border(WindowBorder::Normal), outerDimensions(false)
			, enableDoubleClick(true), toolWindow(false)
		{ }

		HINSTANCE module; /**< Instance to the local module. */
		HMONITOR monitor; /**< Handle ot the monitor onto which to display the window. */
		HWND parent; /**< Optional handle to the parent window if this window is to be a child of an existing window. */
		HWND external; /**< Optional external window handle if the window was created externally. */
		void* creationParams; /**< Parameter that will be passed through the WM_CREATE message. */
		UINT32 width; /**< Width of the window in pixels. */
		UINT32 height; /**< Height of the window in pixels. */
		bool fullscreen; /**< Should the window be opened in fullscreen mode. */
		bool hidden; /**< Should the window be hidden initially. */
		INT32 left; /**< Window origin on X axis in pixels. -1 == screen center. Relative to provided monitor. */
		INT32 top; /**< Window origin on Y axis in pixels. -1 == screen center. Relative to provided monitor. */
		String title; /**< Title of the window. */
		WindowBorder border; /**< Type of border to create the window with. */
		bool outerDimensions; /**< Do our dimensions include space for things like title-bar and border. */
		bool enableDoubleClick; /**< Does window accept double-clicks. */
		bool toolWindow; /**< Tool windows have a different style than normal windows and can be created with no border or title bar. */
	};

	/**
	 * @brief	Represents a Windows native window.
	 */
	class BS_CORE_EXPORT Win32Window
	{
	public:
		Win32Window(const WINDOW_DESC& desc);
		~Win32Window();

		/**
         * @brief	Returns position of the left-most border of the window, relative to the screen.
         */
		INT32 getLeft() const;

		/**
         * @brief	Returns position of the top-most border of the window, relative to the screen.
         */
		INT32 getTop() const;

		/**
         * @brief	Returns width of the window in pixels.
         */
		UINT32 getWidth() const;

		/**
         * @brief	Returns height of the window in pixels.
         */
		UINT32 getHeight() const;

		/**
         * @brief	Returns the native window handle.
         */
		HWND getHWnd() const;

        /**
         * @brief	Hide or show the window.
         */
        void setHidden(bool hidden);

		/**
		 * @brief	Restores or minimizes the window.
		 */
		void setActive(bool state);

		/**
		 * @brief	Minimizes the window to the taskbar.
		 */
		void minimize();

		/**
		 * @brief	Maximizes the window over the entire current screen.
		 */
		void maximize();

		/**
		 * @brief	Restores the window to original position and size if it is
		 *			minimized or maximized.
		 */
		void restore();

        /**
         * @brief	Change the size of the window.
         */
        void resize(UINT32 width, UINT32 height);

        /**
         * @brief	Reposition the window.
         */
		void move(INT32 top, INT32 left);

		/**
		 * @brief	Converts screen position into window local position.
		 */
		Vector2I screenToWindowPos(const Vector2I& screenPos) const;

		/**
		 * @brief	Converts window local position to screen position.
		 */
		Vector2I windowToScreenPos(const Vector2I& windowPos) const;

		/**
		 * @brief	Returns the window style flags used for creating it.
		 */
		DWORD getStyle() const;

		/**
		 * @brief	Returns the extended window style flags used for creating it.
		 */
		DWORD getStyleEx() const;

		/**
		 * @brief	Called when window is moved or resized externally.
		 *
		 * @note	Internal method.
		 */
		void _windowMovedOrResized();

	private:
		struct Pimpl;
		Pimpl* m;
	};
}
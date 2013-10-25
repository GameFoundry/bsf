#pragma once

#include "CmPrerequisites.h"
#include "CmInputFwd.h"
#include "CmInt2.h"
#include "CmRect.h"
#include <boost/signals.hpp>

namespace CamelotFramework
{
	// Encapsulate native cursor type so we can avoid including windows.h as it pollutes the global namespace
	struct CM_EXPORT NativeCursorData
	{
		struct Pimpl;

		NativeCursorData();
		~NativeCursorData();

		Pimpl* data;
	};

	struct CM_EXPORT NonClientResizeArea
	{
		NonClientAreaBorderType type;
		Rect area;
	};

	struct CM_EXPORT WindowNonClientAreaData
	{
		Vector<NonClientResizeArea>::type resizeAreas;
		Vector<Rect>::type moveAreas;
	};

	/**
	 * @brief	Provides access to various Windows operating system functions, including
	 * 			the main message pump.
	 */
	class CM_EXPORT Platform
	{
	public:
		Platform() { }
		virtual ~Platform() { }

		/**
		 * @brief	Moves the cursor to the specified screen position.
		 * 			
		 * @note	Thread safe. 
		 */
		static void setCursorPosition(const Int2& screenPos);

		/**
		 * @brief	Capture mouse to this window so that we get mouse input even if the mouse leaves the window area.
		 *
		 * @note	Thread safe.
		 */
		static void captureMouse(const RenderWindow& window);
		/**
		 * @brief	Releases the mouse capture set by "captureMouse"
		 * 			
		 * @note	Thread safe.
		 */
		static void releaseMouseCapture();

		/**
		 * @brief	Checks if provided over screen position is over the specified window.
		 */
		static bool isPointOverWindow(const RenderWindow& window, const Int2& screenPos);

		/**
		 * @brief	Limit cursor movement to the specified window.
		 *
		 * @note	Thread safe.
		 */
		static void clipCursorToWindow(const RenderWindow& window);
		/**
		 * @brief	Clip cursor to specific area on the screen.
		 *
		 * @note	Thread safe.
		 */
		static void clipCursorToRect(const Rect& screenRect);
		/**
		 * @brief	Disables cursor clipping.
		 * 			
		 * @note	Thread safe.
		 */
		static void clipCursorDisable();

		/**
		 * @brief	Hides the cursor.
		 * 			
		 * @note	Thread safe.
		 */
		static void hideCursor();
		/**
		 * @brief	Shows the cursor.
		 * 			
		 * @note	Thread safe.
		 */
		static void showCursor();

		/**
		 * @brief	Query if the cursor is hidden.
		 *
		 * @note	Thread safe.
		 */
		static bool isCursorHidden() { return mIsCursorHidden; }
		
		/**
		 * @brief	Sets a cursor icon. Uses built-in platform cursor types.
		 *
		 * @note	Thread safe.
		 */
		static void setCursor(CursorType type);
		/**
		 * @brief	Sets a cursor using a custom image.
		 *
		 * @param 	pixelData	Cursor image data.
		 * @param	hotSpot		Offset on the cursor image to where the actual input happens (e.g. tip of the Arrow cursor).
		 * 						
		 * @note	Thread safe.
		 */
		static void setCustomCursor(PixelData& pixelData, const Int2& hotSpot);

		/**
		 * @brief	Sets custom caption non client areas for the specified window. Using custom client
		 * 			areas will override window move/drag operation and trigger when user interacts
		 * 			with the custom area.
		 * 			
		 * @note	Thread safe.
		 * 			All provided areas are relative to the specified window.
		 * 			Mostly useful for frameless windows that don't have typical caption bar.
		 */
		static void setCaptionNonClientAreas(const RenderWindow& window, const Vector<Rect>::type& nonClientAreas);

		/**
		 * @brief	Sets custom non client areas for the specified window. Using custom client
		 * 			areas will override window resize operation and trigger when user interacts
		 * 			with the custom area.
		 * 			
		 * @note	Thread safe.
		 * 			All provided areas are relative to the specified window.
		 * 			Mostly useful for frameless windows that don't have typical border.
		 */
		static void setResizeNonClientAreas(const RenderWindow& window, const Vector<NonClientResizeArea>::type& nonClientAreas);

		/**
		 * @brief	Resets the non client areas for the specified windows and allows 
		 * 			the platform to use the default values.
		 * 			
		 * @note	Thread safe.
		 */
		static void resetNonClientAreas(const RenderWindow& window);

		/**
		 * @brief	Adds a string to the clipboard.
		 */
		static void copyToClipboard(const WString& string);

		/**
		 * @brief	Reads a string from the clipboard and returns it. If there is no
		 * 			string in the clipboard it returns an empty string.
		 * 			
		 * @note	Both wide and normal strings will be read, but normal strings will be converted to
		 * 			a wide string before returning.
		 */
		static WString copyFromClipboard();

		/**
		 * @brief	Queries the internal system performance counter you can use for very precise time
		 * 			measurements. Value is in milliseconds.
		 */
		static double queryPerformanceTimerMs();

		/**
		 * @brief	Message pump. Processes OS messages and returns when it's free.
		 * 			
		 * @note	This method must be called from the core thread.
		 * 			Internal method.
		 */
		static void messagePump();

		/**
		 * @brief	Called once per frame from the sim thread.
		 * 			
		 * @note	Internal method.
		 */
		static void update();

		// Callbacks triggered on the sim thread
		static boost::signal<void(RenderWindow*)> onMouseLeftWindow;

		// Callbacks triggered on the core thread. Be careful so that none
		// of the connected methods call methods intended for sim thread.
		static boost::signal<void(const Int2&, OSPositionalInputButtonStates)> onCursorMoved;
		static boost::signal<void(const Int2&, OSMouseButton button, OSPositionalInputButtonStates)> onCursorButtonPressed;
		static boost::signal<void(const Int2&, OSMouseButton button, OSPositionalInputButtonStates)> onCursorButtonReleased;
		static boost::signal<void(const Int2&, OSPositionalInputButtonStates)> onCursorDoubleClick;
		static boost::signal<void(InputCommandType)> onInputCommand;
		static boost::signal<void(float)> onMouseWheelScrolled;
		static boost::signal<void(UINT32)> onCharInput;
		static boost::signal<void(RenderWindow*)> onWindowFocusReceived;
		static boost::signal<void(RenderWindow*)> onWindowFocusLost;
		static boost::signal<void(RenderWindow*)> onWindowMovedOrResized;
		static boost::signal<void()> onMouseCaptureChanged;
	protected:
		static bool mIsCursorHidden;
		static NativeCursorData mCursor;
		static bool mUsingCustomCursor;
		static Map<const RenderWindow*, WindowNonClientAreaData>::type mNonClientAreas;

		static bool mIsTrackingMouse;
		static Vector<RenderWindow*>::type mMouseLeftWindows;

		CM_STATIC_MUTEX(mSync);

		static void win32ShowCursor();
		static void win32HideCursor();

		static void windowFocusReceived(RenderWindow* window);
		static void windowFocusLost(RenderWindow* window);
		static void windowMovedOrResized(RenderWindow* window);
	};
}
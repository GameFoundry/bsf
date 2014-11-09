#pragma once

#include "BsCorePrerequisites.h"
#include "BsInputFwd.h"
#include "BsVector2I.h"
#include "BsRect2I.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	/**
	 * @brief	Encapsulate native cursor data so we can avoid including windows.h as it pollutes the global namespace
	 */
	struct BS_CORE_EXPORT NativeCursorData
	{
		struct Pimpl;

		NativeCursorData();
		~NativeCursorData();

		Pimpl* data;
	};

	/**
	 * @brief	Encapsulate drop target data so we can avoid including windows.h as it pollutes the global namespace
	 */
	struct BS_CORE_EXPORT NativeDropTargetData
	{
		struct Pimpl;

		NativeDropTargetData();
		~NativeDropTargetData();

		Pimpl* data;
	};

	/**
	 * @brief	Represents a specific non client area used for window resizing.
	 */
	struct BS_CORE_EXPORT NonClientResizeArea
	{
		NonClientAreaBorderType type;
		Rect2I area;
	};

	/**
	 * @brief	Contains a list of window move and resize non client areas.
	 */
	struct BS_CORE_EXPORT WindowNonClientAreaData
	{
		Vector<NonClientResizeArea> resizeAreas;
		Vector<Rect2I> moveAreas;
	};

	/**
	 * @brief	Provides access to various Windows operating system functions, including
	 * 			the main message pump.
	 */
	class BS_CORE_EXPORT Platform
	{
	public:
		Platform() { }
		virtual ~Platform() { }

		/**
		 * @brief	Retrieves the cursor position in screen coordinates.
		 * 			
		 * @note	Thread safe. 
		 */
		static Vector2I getCursorPosition();

		/**
		 * @brief	Moves the cursor to the specified screen position.
		 * 			
		 * @note	Thread safe. 
		 */
		static void setCursorPosition(const Vector2I& screenPos);

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
		static bool isPointOverWindow(const RenderWindow& window, const Vector2I& screenPos);

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
		static void clipCursorToRect(const Rect2I& screenRect);
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
		 * @brief	Sets a cursor using a custom image.
		 *
		 * @param 	pixelData	Cursor image data.
		 * @param	hotSpot		Offset on the cursor image to where the actual input happens (e.g. tip of the Arrow cursor).
		 * 						
		 * @note	Thread safe.
		 */
		static void setCursor(PixelData& pixelData, const Vector2I& hotSpot);

		/**
		 * @brief	Sets custom caption non client areas for the specified window. Using custom client
		 * 			areas will override window move/drag operation and trigger when user interacts
		 * 			with the custom area.
		 * 			
		 * @note	Thread safe.
		 * 			All provided areas are relative to the specified window.
		 * 			Mostly useful for frameless windows that don't have typical caption bar.
		 */
		static void setCaptionNonClientAreas(const RenderWindowCore& window, const Vector<Rect2I>& nonClientAreas);

		/**
		 * @brief	Sets custom non client areas for the specified window. Using custom client
		 * 			areas will override window resize operation and trigger when user interacts
		 * 			with the custom area.
		 * 			
		 * @note	Thread safe.
		 * 			All provided areas are relative to the specified window.
		 * 			Mostly useful for frameless windows that don't have typical border.
		 */
		static void setResizeNonClientAreas(const RenderWindowCore& window, const Vector<NonClientResizeArea>& nonClientAreas);

		/**
		 * @brief	Resets the non client areas for the specified windows and allows 
		 * 			the platform to use the default values.
		 * 			
		 * @note	Thread safe.
		 */
		static void resetNonClientAreas(const RenderWindowCore& window);

		/**
		 * @brief	Adds a string to the clipboard.
		 * 			
		 * @note	Thread safe.
		 */
		static void copyToClipboard(const WString& string);

		/**
		 * @brief	Reads a string from the clipboard and returns it. If there is no
		 * 			string in the clipboard it returns an empty string.
		 * 			
		 * @note	Both wide and normal strings will be read, but normal strings will be converted to
		 * 			a wide string before returning.
		 * 			
		 *			Thread safe.
		 */
		static WString copyFromClipboard();

		/**
		 * @brief	Populates the provided buffer with a MAC address of the first available
		 *			adapter, if one exists. If no adapters exist, returns false.
		 */
		static bool getMACAddress(MACAddress& address);

		/**
		 * @brief	Queries the internal system performance counter you can use for very precise time
		 * 			measurements. Value is in milliseconds.
		 * 			
		 * @note	Thread safe.
		 */
		static double queryPerformanceTimerMs();

		/**
		 * @brief	Creates a drop target that you can use for tracking OS drag and drop operations performed over
		 * 			a certain area on the specified window.
		 *
		 * @param	window	The window on which to track drop operations.
		 * @param	x	  	The x coordinate of the area to track, relative to window.
		 * @param	y	  	The y coordinate of the area to track, relative to window.
		 * @param	width 	The width of the area to track.
		 * @param	height	The height of the area to track.
		 *
		 * @return	OSDropTarget that you will use to receive all drop data. When no longer needed make sure to destroy it with
		 * 			destroyDropTarget().
		 */
		static OSDropTarget& createDropTarget(const RenderWindow* window, int x, int y, unsigned int width, unsigned int height);

		/**
		 * @brief	Destroys a drop target previously created with createDropTarget.
		 */
		static void destroyDropTarget(OSDropTarget& target);

		/**
		 * @brief	Message pump. Processes OS messages and returns when it's free.
		 * 			
		 * @note	Internal method.
		 * 			Core thread only.
		 */
		static void _messagePump();

		/**
		 * @brief	Called during application start up from the sim thread.
		 * 			Must be called before any other operations are done.
		 * 			
		 * @note	Internal method.
		 */
		static void _startUp();

		/**
		 * @brief	Called once per frame from the sim thread.
		 * 			
		 * @note	Internal method.
		 * 			Sim thread only.
		 */
		static void _update();

		/**
		 * @brief	Called once per frame from the core thread.
		 * 			
		 * @note	Internal method.
		 * 			Core thread only.
		 */
		static void _coreUpdate();

		/**
		 * @brief	Called during application shut down from the sim thread.
		 * 			
		 * @note	Internal method.
		 * 			Sim thread only.
		 */
		static void _shutDown();

		/**
		 * @brief	Triggered when a pointer leaves the provided window.
		 * 			
		 * @note	Sim thread only.
		 */
		static Event<void(RenderWindowCore*)> onMouseLeftWindow;

		/**
		 * @brief	Triggered whenever the pointer moves.
		 * 			
		 * @note	Core thread only.
		 */
		static Event<void(const Vector2I&, OSPointerButtonStates)> onCursorMoved;

		/**
		 * @brief	Triggered whenever a pointer button is pressed.
		 * 			
		 * @note	Core thread only.
		 */
		static Event<void(const Vector2I&, OSMouseButton button, OSPointerButtonStates)> onCursorButtonPressed;

		/**
		 * @brief	Triggered whenever pointer button is released.
		 * 			
		 * @note	Core thread only.
		 */
		static Event<void(const Vector2I&, OSMouseButton button, OSPointerButtonStates)> onCursorButtonReleased;

		/**
		 * @brief	Triggered whenever a pointer button is double clicked.
		 * 			
		 * @note	Core thread only.
		 */
		static Event<void(const Vector2I&, OSPointerButtonStates)> onCursorDoubleClick;

		/**
		 * @brief	Triggered whenever an input command is entered.
		 * 			
		 * @note	Core thread only.
		 */
		static Event<void(InputCommandType)> onInputCommand;

		/**
		 * @brief	Triggered whenever the mouse wheel is scolled.
		 * 			
		 * @note	Core thread only.
		 */
		static Event<void(float)> onMouseWheelScrolled;

		/**
		 * @brief	Triggered whenever a character is entered.
		 * 			
		 * @note	Core thread only.
		 */
		static Event<void(UINT32)> onCharInput;

		/**
		 * @brief	Triggered whenever a window receives focus.
		 * 			
		 * @note	Core thread only.
		 */
		static Event<void(RenderWindowCore*)> onWindowFocusReceived;

		/**
		 * @brief	Triggered whenever a window loses focus.
		 * 			
		 * @note	Core thread only.
		 */
		static Event<void(RenderWindowCore*)> onWindowFocusLost;

		/**
		 * @brief	Triggered whenever a window gets moved or resized.
		 * 			
		 * @note	Core thread only.
		 */
		static Event<void(RenderWindowCore*)> onWindowMovedOrResized;

		/**
		 * @brief	Triggered whenever mouse capture state for the window is changed
		 * 			(it receives or loses it).
		 * 			
		 * @note	Core thread only.
		 */
		static Event<void()> onMouseCaptureChanged;
	protected:
		static bool mIsCursorHidden;
		static NativeCursorData mCursor;
		static bool mUsingCustomCursor;
		static Map<const RenderWindowCore*, WindowNonClientAreaData> mNonClientAreas;

		static bool mIsTrackingMouse;
		static Stack<RenderWindowCore*> mModalWindowStack;

		static NativeDropTargetData mDropTargets;

		static bool mRequiresStartUp;
		static bool mRequiresShutDown;

		BS_STATIC_MUTEX(mSync);

		static void win32ShowCursor();
		static void win32HideCursor();

		static void windowFocusReceived(RenderWindowCore* window);
		static void windowFocusLost(RenderWindowCore* window);
		static void windowMovedOrResized(RenderWindowCore* window);
	};
}
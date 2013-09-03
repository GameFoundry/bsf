#pragma once

#include "CmPrerequisites.h"
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

	/**
	 * @brief	Provides access for version Windows operating system functions, including
	 * 			the main message pump.
	 */
	class CM_EXPORT Platform
	{
	public:
		Platform() { }
		virtual ~Platform() { }

		/**
		 * @brief	Moves the cursor to the specified screen position.
		 */
		static void setCursorPosition(const Int2& screenPos);

		static void captureMouse(const RenderWindow& window);
		static void releaseMouseCapture();

		static void clipCursorToWindow(const RenderWindow& window);
		static void clipCursorToRect(const Rect& screenRect);
		static void clipCursorDisable();

		static void hideCursor();
		static void showCursor();

		static bool isCursorHidden() { return mIsCursorHidden; }
		
		static void setCursor(CursorType type);
		static void setCustomCursor(PixelData& pixelData, const Int2& hotSpot);

		/**
		 * @brief	Message pump. Processes OS messages and returns when it's free.
		 * 			
		 * @note	This method must be called from the core thread.
		 */
		static void messagePump();

		static boost::signal<void(const Int2&)> onMouseMoved;
		static boost::signal<void(float)> onMouseWheelScrolled;
		static boost::signal<void(UINT32)> onCharInput;
		static boost::signal<void(RenderWindow*)> onWindowFocusReceived;
		static boost::signal<void(RenderWindow*)> onWindowFocusLost;
		static boost::signal<void(RenderWindow*)> onWindowMovedOrResized;
	protected:
		static bool mIsCursorHidden;
		static NativeCursorData mCursor;
		static bool mUsingCustomCursor;

		static void win32ShowCursor();
		static void win32HideCursor();

		static void windowFocusReceived(RenderWindow* window);
		static void windowFocusLost(RenderWindow* window);
		static void windowMovedOrResized(RenderWindow* window);
	};
}
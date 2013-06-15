#pragma once

#include "CmPrerequisites.h"
#include "CmInt2.h"
#include "CmRect.h"

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
	 * @brief	Provides controls for Windows operating system cursor.
	 */
	class CM_EXPORT Cursor
	{
	public:
		Cursor();
		~Cursor();

		/**
		 * @brief	Moves the cursor to the specified screen position.
		 */
		static void setPosition(const Int2& screenPos);

		static void clipToWindow(const RenderWindow& window);
		static void clipToRect(const Rect& screenRect);
		static void clipDisable();

		static void hide();
		static void show();

		static bool isHidden() { return mIsHidden; }
		
		static void setCursor(CursorType type);
		static void setCustomCursor(PixelData& pixelData, const Int2& hotSpot);

		static void _win32ShowCursor();
		static void _win32HideCursor();
		
	private:
		static bool mIsHidden;
		static NativeCursorData mCursor;
		static bool mUsingCustom;
	};
}
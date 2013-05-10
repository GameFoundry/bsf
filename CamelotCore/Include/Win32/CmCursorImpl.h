#pragma once

#include "CmPrerequisites.h"
#include "CmInt2.h"
#include "CmRect.h"
#include <windows.h>

namespace CamelotFramework
{
	/**
	 * @brief	Provides controls for Windows operating system cursor.
	 */
	class CM_EXPORT Cursor
	{
	public:
		static Int2 getScreenPosition();
		static void setScreenPosition(const Int2& pos);

		static Int2 getWindowPosition(const RenderWindow& window);
		static void setWindowPosition(const RenderWindow& window, const Int2& pos);

		static void clipToWindow(const RenderWindow& window);
		static void clipToRect(const Rect& screenRect);
		static void clipDisable();

		static void hide();
		static void show();

		static bool isHidden() { return mIsHidden; }
		
		static void setCursor(CursorType type);
		static void setCustomCursor(PixelData& pixelData, const Int2& hotSpot);
		static HCURSOR getHCursor() { return mCursor; }

	private:
		static bool mIsHidden;
		static HCURSOR mCursor;
		static bool mUsingCustom;
	};
}
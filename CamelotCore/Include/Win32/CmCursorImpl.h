#pragma once

#include "CmPrerequisites.h"
#include "CmInt2.h"
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

		static Int2 getWindowPosition(RenderWindow& window);
		static void setWindowPosition(RenderWindow& window, const Int2& pos);

		static void hide();
		static void show();
	};
}
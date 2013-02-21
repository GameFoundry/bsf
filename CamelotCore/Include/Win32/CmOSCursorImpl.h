#pragma once

#include "CmPrerequisites.h"
#include "CmInt2.h"
#include <windows.h>

namespace CamelotEngine
{
	/**
	 * @brief	Provides controls for Windows operating system cursor.
	 */
	class CM_EXPORT OSCursor
	{
	public:
		static Int2 getPosition();
		static void setPosition(const Int2& pos);

		static void hide();
		static void show();
	};
}
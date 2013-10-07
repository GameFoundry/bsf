#pragma once

#include "CmPrerequisites.h"

namespace CamelotFramework
{
	enum class CursorType
	{
		Arrow,
		Wait,
		IBeam,
		Help,
		Hand,
		SizeAll,
		SizeNESW,
		SizeNS,
		SizeNWSE,
		SizeWE
	};

	enum class NonClientAreaBorderType
	{
		TopLeft,
		Top,
		TopRight,
		Left,
		Right,
		BottomLeft,
		Bottom,
		BottomRight	
	};

	enum class OSMouseButton
	{
		Left, Middle, Right, Count
	};

	struct CM_EXPORT OSPositionalInputButtonStates
	{
		OSPositionalInputButtonStates()
		{
			mouseButtons[0] = false;
			mouseButtons[1] = false;
			mouseButtons[2] = false;

			shift = false;
			ctrl = false;
		}

		bool mouseButtons[OSMouseButton::Count];
		bool shift, ctrl;
	};
}

//Bring in the specific platform's header file
#if CM_PLATFORM == CM_PLATFORM_WIN32
# include "Win32/CmPlatformImpl.h"
#elif (CM_PLATFORM == CM_PLATFORM_LINUX)
# include "GLX/CmPlatformImpl.h"
#elif CM_PLATFORM == CM_PLATFORM_APPLE
# include "OSX/CmPlatformImpl.h"
#endif

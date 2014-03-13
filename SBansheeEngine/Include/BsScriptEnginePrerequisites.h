#pragma once

#include "BsMonoPrerequisites.h"

#if (CM_PLATFORM == CM_PLATFORM_WIN32) && !defined(__MINGW32__)
#	ifdef BS_SCR_BE_EXPORTS
#		define BS_SCR_BE_EXPORT __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define BS_SCR_BE_EXPORT
#       else
#    		define BS_SCR_BE_EXPORT __declspec(dllimport)
#       endif
#	endif
#elif defined ( CM_GCC_VISIBILITY )
#    define BS_SCR_BE_EXPORT  __attribute__ ((visibility("default")))
#else
#    define BS_SCR_BE_EXPORT
#endif

namespace BansheeEngine
{
	class ScriptFont;
	class ScriptSpriteTexture;
	class ScriptTexture2D;
	class ScriptGUIElementStyle;
	class ScriptGUIElementStateStyle;
	class ScriptGUIBase;
	class ScriptGUIArea;
	class ScriptGUILayout;
	class ScriptGUILabel;
	class ScriptComponent;
	class ManagedComponent;

	enum TypeID_BansheeScript
	{
		TID_ManagedComponent = 50000
	};

	static const char* BansheeEngineAssemblyName = "MBansheeEngine";
}
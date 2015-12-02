#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsEditorPrerequisites.h"

#if (BS_PLATFORM == BS_PLATFORM_WIN32) && !defined(__MINGW32__)
#	ifdef BS_SCR_BED_EXPORTS
#		define BS_SCR_BED_EXPORT __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define BS_SCR_BED_EXPORT
#       else
#    		define BS_SCR_BED_EXPORT __declspec(dllimport)
#       endif
#	endif
#elif defined ( BS_GCC_VISIBILITY )
#    define BS_SCR_BED_EXPORT  __attribute__ ((visibility("default")))
#else
#    define BS_SCR_BED_EXPORT
#endif

namespace BansheeEngine
{
	class ScriptEditorWindow;
	class GUIGameObjectField;
	class GUIResourceField;
	class GUITextureField;
	class ScriptHandleSliderBase;
	class ScriptPlatformInfo;
}
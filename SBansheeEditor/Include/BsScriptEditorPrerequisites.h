#pragma once

#include "BsScriptEnginePrerequisites.h"

#if (CM_PLATFORM == CM_PLATFORM_WIN32) && !defined(__MINGW32__)
#	ifdef BS_SCR_BED_EXPORTS
#		define BS_SCR_BED_EXPORT __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define BS_SCR_BED_EXPORT
#       else
#    		define BS_SCR_BED_EXPORT __declspec(dllimport)
#       endif
#	endif
#elif defined ( CM_GCC_VISIBILITY )
#    define BS_SCR_BED_EXPORT  __attribute__ ((visibility("default")))
#else
#    define BS_SCR_BED_EXPORT
#endif

namespace BansheeEditor
{
	class ScriptModalWindow;
}
#pragma once

#include "BsCorePrerequisites.h"

#if (BS_PLATFORM == BS_PLATFORM_WIN32) && !defined(__MINGW32__)
#	ifdef BS_FONT_EXPORTS
#		define BS_FONT_EXPORT __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define BS_FONT_EXPORT
#       else
#    		define BS_FONT_EXPORT __declspec(dllimport)
#       endif
#	endif
#elif defined ( BS_GCC_VISIBILITY )
#    define BS_FONT_EXPORT  __attribute__ ((visibility("default")))
#else
#    define BS_FONT_EXPORT
#endif
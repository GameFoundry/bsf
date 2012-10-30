#pragma once

#include "CmPrerequisites.h"

#if (CM_PLATFORM == CM_PLATFORM_WIN32) && !defined(__MINGW32__)
#	ifdef CM_FWDRND_EXPORTS
#		define CM_FWDRND_EXPORT __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define CM_FWDRND_EXPORT
#       else
#    		define CM_FWDRND_EXPORT __declspec(dllimport)
#       endif
#	endif
#elif defined ( CM_GCC_VISIBILITY )
#    define CM_FWDRND_EXPORT  __attribute__ ((visibility("default")))
#else
#    define CM_FWDRND_EXPORT
#endif
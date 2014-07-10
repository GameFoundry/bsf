//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsCorePrerequisites.h"

#if (BS_PLATFORM == BS_PLATFORM_WIN32) && !defined(__MINGW32__)
#	ifdef BS_FBX_EXPORTS
#		define BS_FBX_EXPORT __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define BS_FBX_EXPORT
#       else
#    		define BS_FBX_EXPORT __declspec(dllimport)
#       endif
#	endif
#elif defined ( BS_GCC_VISIBILITY )
#    define BS_FBX_EXPORT  __attribute__ ((visibility("default")))
#else
#    define BS_FBX_EXPORT
#endif
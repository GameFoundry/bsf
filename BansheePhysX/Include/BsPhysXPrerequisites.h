//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"

namespace BansheeEngine
{
#if (BS_PLATFORM == BS_PLATFORM_WIN32) && !defined(BS_STATIC_LIB)
#	ifdef BS_PHYSX_EXPORTS
#		define BS_PHYSX_EXPORT __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define BS_PHYSX_EXPORT
#       else
#    		define BS_PHYSX_EXPORT __declspec(dllimport)
#       endif
#	endif
#else
#	define BS_PHYSX_EXPORT
#endif
}
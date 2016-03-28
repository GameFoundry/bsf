//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
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

/** @addtogroup Plugins
 *  @{
 */

/** @defgroup FBX BansheeFBXImporter
 *	%Mesh importer for the FBX file format.
 */

/** @} */

#define FBXSDK_NEW_API
#include <fbxsdk.h>

#define FBX_IMPORT_MAX_UV_LAYERS 2
#define FBX_IMPORT_MAX_BONE_INFLUENCES 4
//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include <assert.h>

// 0 - No thread support
// 1 - Render system is thread safe (TODO: NOT WORKING and will probably be removed)
// 2 - Thread support but render system can only be accessed from main thread
#define BS_THREAD_SUPPORT 2

#define BS_PROFILING_ENABLED 1

// Versions

#define BS_VER_DEV 1
#define BS_VER_PREVIEW 2

#define BS_VER VS_VER_PREVIEW

// Platform-specific stuff
#include "BsPlatformDefines.h"

#if BS_COMPILER == BS_COMPILER_MSVC

// TODO - This is not deactivated anywhere, therefore it applies to any file that includes this header.
//      - Right now I don't have an easier way to apply these warnings globally so I'm keeping it this way.

// Secure versions aren't multiplatform, so we won't be using them
#define _CRT_SECURE_NO_WARNINGS

// disable: "<type> needs to have dll-interface to be used by clients'
// Happens on STL member variables which are not public therefore is ok
#   pragma warning (disable: 4251)

// disable: 'X' Function call with parameters that may be unsafe
#	pragma warning(disable: 4996) 

// disable: decorated name length exceeded, name was truncated
// Happens with really long type names. Even fairly standard use
// of std::unordered_map with custom parameters, meaning I can't
// really do much to avoid it. It shouldn't effect execution
// but might cause problems if you compile library
// with one compiler and use it in another.
#	pragma warning(disable: 4503)

#endif

// Short-hand names for various built-in types
#include "BsTypes.h"

#include "BsMemoryAllocator.h"

// Useful threading defines
#include "BsThreadDefines.h"

// Commonly used standard headers
#include "BsStdHeaders.h"

// Forward declarations
#include "BsFwdDeclUtil.h"

#include "BsRTTIPrerequisites.h"

#include "BsString.h"
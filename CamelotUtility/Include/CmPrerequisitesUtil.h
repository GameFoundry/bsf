#pragma once

#include <assert.h>

// 0 - No thread support
// 1 - Render system is thread safe (TODO: NOT WORKING and will probably be removed)
// 2 - Thread support but render system can only be accessed from main thread
#define CM_THREAD_SUPPORT 2

#define CM_PROFILING_ENABLED 1

// Platform-specific stuff
#include "CmPlatformDefines.h"

#if CM_COMPILER == CM_COMPILER_MSVC

// TODO - This is not deactivated anywhere, therefore it applies to any file that includes this header.
//      - Right now I don't have an easier way to apply these warnings globally so I'm keeping it this way.

// Secure versions aren't multiplatform, so we won't be using them
#define _CRT_SECURE_NO_WARNINGS

// Disable: "<type> needs to have dll-interface to be used by clients'
// Happens on STL member variables which are not public therefore is ok
#   pragma warning (disable: 4251)

#endif

// Short-hand names for various built-in types
#include "CmTypes.h"

#include "CmMemoryAllocator.h"

// Useful threading defines
#include "CmThreadDefines.h"

// Commonly used standard headers
#include "CmStdHeaders.h"

// Forward declarations
#include "CmFwdDeclUtil.h"

#include "CmRTTIPrerequisites.h"

#include "CmString.h"
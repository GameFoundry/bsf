//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include <assert.h>

/** @defgroup Utility Utility
 *	Lowest layer of the engine containing a collection of very decoupled and separate systems that are 
 *  likely to be used throughout all of the higher layers.
 *  @{
 */

/** @defgroup Math Math
 *	Math utility library containing a variety of general purpose math functionality.
 */

/** @defgroup RTTI RTTI
 *  A set of systems for defining and using run-time type information.
 */

 /** @defgroup Serialization Serialization
 *  A set of systems for serializing and deserializing native objects.
 */

/** @defgroup Memory Memory
 *  A set of methods and classes meant to manipulate memory.
 */

/** @defgroup Containers Containers
 *  Contains a set of templated commonly used containers.
 */

/** @defgroup Debug Debug
 *  Contains functionality used to help with debugging.
 */

/** @defgroup Error Error handling
 *  Contains functionality used for handling and reporting errors.
 */

/** @defgroup Filesystem File system
 *  Contains functionality used for manipulating, reading and writing files.
 */

/** @defgroup General General
 *  Contains general utility functionality that doesn't fit in any other category.
 */

/** @defgroup Image Image
 *  Contains various utility methods for manipulating images.
 */

/** @defgroup String String
 *  Contains functionality for manipulating strings.
 */

/** @defgroup Testing Testing
 *  Contains functionality for running unit tests.
 */

 /** @defgroup Threading Threading
 *  Contains functionality for manipulating threads and thread synchronization.
 */

/** @defgroup RTTI-Impl-Utility RTTI types
 *  Types containing RTTI for specific classes.
 */

/** @defgroup Platform-Utility Platform
 *  Platform specific functionality.
 */

/** @} */

/** @defgroup Plugins Plugins
 *	Contains all the interchangeable high level systems that are built upon abstractions provided by the engine core.
 */

/** @defgroup Implementation Implementation
 *	Contains various base and helper types that used by an implementation of some other type. These shouldn't even be part
 *  of the class list but due to limitations in the documentation generation system they need to be somewhere. All elements 
 *  listed here should instead be found by browsing the public interfaces of the types that use them.
 */

// 0 - No thread support
// 1 - Render system is thread safe (TODO: NOT WORKING and will probably be removed)
// 2 - Thread support but render system can only be accessed from main thread
#define BS_THREAD_SUPPORT 2

#define BS_PROFILING_ENABLED 1

// Versions

#define BS_VER_DEV 1
#define BS_VER_PREVIEW 2

#define BS_VER BS_VER_DEV

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

// disable: C++ exception handler used, but unwind semantics are not enabled
// We don't care about this as any exception is meant to crash the program.
#	pragma warning(disable: 4530)

#endif

// Windows Settings
#if BS_PLATFORM == BS_PLATFORM_WIN32
// Win32 compilers use _DEBUG for specifying debug builds.
// for MinGW, we set DEBUG
#   if defined(_DEBUG) || defined(DEBUG)
#       define BS_DEBUG_MODE 1
#   else
#       define BS_DEBUG_MODE 0
#   endif

#endif

// Linux/Apple Settings
#if BS_PLATFORM == BS_PLATFORM_LINUX || BS_PLATFORM == BS_PLATFORM_OSX
// A quick define to overcome different names for the same function
#   define stricmp strcasecmp

#   ifdef DEBUG
#       define BS_DEBUG_MODE 1
#   else
#       define BS_DEBUG_MODE 0
#   endif

#endif

#if BS_DEBUG_MODE
#define BS_DEBUG_ONLY(x) x
#define BS_ASSERT(x) assert(x)
#else
#define BS_DEBUG_ONLY(x)
#define BS_ASSERT(x)
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

#include "BsFlags.h"
#include "BsString.h"
#include "BsMessageHandlerFwd.h"
#include "BsUtil.h"
#include "BsPath.h"
#include "BsStringID.h"
#include "BsEvent.h"
#include "BsPlatformUtility.h"
#include "BsCrashHandler.h"
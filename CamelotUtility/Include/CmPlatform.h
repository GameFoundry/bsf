/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#pragma once

namespace CamelotEngine {
/* Initial platform/compiler-related stuff to set.
*/
#define OGRE_PLATFORM_WIN32 1
#define OGRE_PLATFORM_LINUX 2
#define OGRE_PLATFORM_APPLE 3

#define OGRE_COMPILER_MSVC 1
#define OGRE_COMPILER_GNUC 2
#define OGRE_COMPILER_BORL 3
#define OGRE_COMPILER_WINSCW 4
#define OGRE_COMPILER_GCCE 5

#define OGRE_ARCHITECTURE_32 1
#define OGRE_ARCHITECTURE_64 2

/* Finds the compiler type and version.
*/
#if defined( __GCCE__ )
#   define OGRE_COMPILER OGRE_COMPILER_GCCE
#   define OGRE_COMP_VER _MSC_VER
//#	include <staticlibinit_gcce.h> // This is a GCCE toolchain workaround needed when compiling with GCCE 
#elif defined( __WINSCW__ )
#   define OGRE_COMPILER OGRE_COMPILER_WINSCW
#   define OGRE_COMP_VER _MSC_VER
#elif defined( _MSC_VER )
#   define OGRE_COMPILER OGRE_COMPILER_MSVC
#   define OGRE_COMP_VER _MSC_VER
#elif defined( __GNUC__ )
#   define OGRE_COMPILER OGRE_COMPILER_GNUC
#   define OGRE_COMP_VER (((__GNUC__)*100) + \
        (__GNUC_MINOR__*10) + \
        __GNUC_PATCHLEVEL__)

#elif defined( __BORLANDC__ )
#   define OGRE_COMPILER OGRE_COMPILER_BORL
#   define OGRE_COMP_VER __BCPLUSPLUS__
#   define __FUNCTION__ __FUNC__ 
#else
#   pragma error "No known compiler. Abort! Abort!"

#endif

/* See if we can use __forceinline or if we need to use __inline instead */
#if OGRE_COMPILER == OGRE_COMPILER_MSVC
#   if OGRE_COMP_VER >= 1200
#       define FORCEINLINE __forceinline
#   endif
#elif defined(__MINGW32__)
#   if !defined(FORCEINLINE)
#       define FORCEINLINE __inline
#   endif
#else
#   define FORCEINLINE __inline
#endif

/* Finds the current platform */

#if defined( __WIN32__ ) || defined( _WIN32 )
#   define OGRE_PLATFORM OGRE_PLATFORM_WIN32
#elif defined( __APPLE_CC__)
#   define OGRE_PLATFORM OGRE_PLATFORM_APPLE
#else
#   define OGRE_PLATFORM OGRE_PLATFORM_LINUX
#endif

    /* Find the arch type */
#if defined(__x86_64__) || defined(_M_X64) || defined(__powerpc64__) || defined(__alpha__) || defined(__ia64__) || defined(__s390__) || defined(__s390x__)
#   define OGRE_ARCH_TYPE OGRE_ARCHITECTURE_64
#else
#   define OGRE_ARCH_TYPE OGRE_ARCHITECTURE_32
#endif

// For generating compiler warnings - should work on any compiler
// As a side note, if you start your message with 'Warning: ', the MSVC
// IDE actually does catch a warning :)
#define OGRE_QUOTE_INPLACE(x) # x
#define OGRE_QUOTE(x) OGRE_QUOTE_INPLACE(x)
#define OGRE_WARN( x )  message( __FILE__ "(" QUOTE( __LINE__ ) ") : " x "\n" )

//----------------------------------------------------------------------------
// Windows Settings
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32

// If we're not including this from a client build, specify that the stuff
// should get exported. Otherwise, import it.
#	if defined( OGRE_STATIC_LIB )
		// Linux compilers don't have symbol import/export directives.
#   	define _OgreExport
#   	define _OgrePrivate
#   else
#   	if defined( OGRE_NONCLIENT_BUILD )
#       	define _OgreExport __declspec( dllexport )
#   	else
#           if defined( __MINGW32__ )
#               define _OgreExport
#           else
#       	    define _OgreExport __declspec( dllimport )
#           endif
#   	endif
#   	define _OgrePrivate
#	endif
// Win32 compilers use _DEBUG for specifying debug builds.
// for MinGW, we set DEBUG
#   if defined(_DEBUG) || defined(DEBUG)
#       define OGRE_DEBUG_MODE 1
#   else
#       define OGRE_DEBUG_MODE 0
#   endif

// Disable unicode support on MingW for GCC 3, poorly supported in stdlibc++
// STLPORT fixes this though so allow if found
// MinGW C++ Toolkit supports unicode and sets the define __MINGW32_TOOLBOX_UNICODE__ in _mingw.h
// GCC 4 is also fine
#if defined(__MINGW32__)
# if OGRE_COMP_VER < 400
#  if !defined(_STLPORT_VERSION)
#   include<_mingw.h>
#   if defined(__MINGW32_TOOLBOX_UNICODE__) || OGRE_COMP_VER > 345
#    define OGRE_UNICODE_SUPPORT 1
#   else
#    define OGRE_UNICODE_SUPPORT 0
#   endif
#  else
#   define OGRE_UNICODE_SUPPORT 1
#  endif
# else
#  define OGRE_UNICODE_SUPPORT 1
# endif
#else
#  define OGRE_UNICODE_SUPPORT 1
#endif

#endif // OGRE_PLATFORM == OGRE_PLATFORM_WIN32

//----------------------------------------------------------------------------
// Linux/Apple/Symbian Settings
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX || OGRE_PLATFORM == OGRE_PLATFORM_APPLE

// Enable GCC symbol visibility
#   if defined( OGRE_GCC_VISIBILITY )
#       define _OgreExport  __attribute__ ((visibility("default")))
#       define _OgrePrivate __attribute__ ((visibility("hidden")))
#   else
#       define _OgreExport
#       define _OgrePrivate
#   endif

// A quick define to overcome different names for the same function
#   define stricmp strcasecmp

// Unlike the Win32 compilers, Linux compilers seem to use DEBUG for when
// specifying a debug build.
// (??? this is wrong, on Linux debug builds aren't marked in any way unless
// you mark it yourself any way you like it -- zap ???)
#   ifdef DEBUG
#       define OGRE_DEBUG_MODE 1
#   else
#       define OGRE_DEBUG_MODE 0
#   endif

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
    #define OGRE_PLATFORM_LIB "CmPlatform.bundle"
#else //OGRE_PLATFORM_LINUX
    #define OGRE_PLATFORM_LIB "libCmPlatform.so"
#endif

#endif

//----------------------------------------------------------------------------

/* Initial CPU stuff to set.
*/
#define OGRE_CPU_UNKNOWN    0
#define OGRE_CPU_X86        1
#define OGRE_CPU_PPC        2

/* Find CPU type
*/
#if (defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))) || \
    (defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__)))
#   define OGRE_CPU OGRE_CPU_X86

#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE && defined(__BIG_ENDIAN__)
#   define OGRE_CPU OGRE_CPU_PPC
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#	define OGRE_CPU OGRE_CPU_X86
#else
#   define OGRE_CPU OGRE_CPU_UNKNOWN
#endif

/* Find how to declare aligned variable.
*/
#if OGRE_COMPILER == OGRE_COMPILER_MSVC
#   define OGRE_ALIGNED_DECL(type, var, alignment)  __declspec(align(alignment)) type var

#elif OGRE_COMPILER == OGRE_COMPILER_GNUC
#   define OGRE_ALIGNED_DECL(type, var, alignment)  type var __attribute__((__aligned__(alignment)))

#else
#   define OGRE_ALIGNED_DECL(type, var, alignment)  type var
#endif

/** Find perfect alignment (should supports SIMD alignment if SIMD available)
*/
#if OGRE_CPU == OGRE_CPU_X86
#   define OGRE_SIMD_ALIGNMENT  16

#else
#   define OGRE_SIMD_ALIGNMENT  16
#endif

/* Declare variable aligned to SIMD alignment.
*/
#define OGRE_SIMD_ALIGNED_DECL(type, var)   OGRE_ALIGNED_DECL(type, var, OGRE_SIMD_ALIGNMENT)

}
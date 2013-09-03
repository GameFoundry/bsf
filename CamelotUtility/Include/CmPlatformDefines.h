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

namespace CamelotFramework {
/* Initial platform/compiler-related stuff to set.
*/
#define CM_PLATFORM_WIN32 1
#define CM_PLATFORM_LINUX 2
#define CM_PLATFORM_APPLE 3

#define CM_COMPILER_MSVC 1
#define CM_COMPILER_GNUC 2
#define CM_COMPILER_BORL 3
#define CM_COMPILER_WINSCW 4
#define CM_COMPILER_GCCE 5

#define CM_ARCHITECTURE_32 1
#define CM_ARCHITECTURE_64 2

#define CM_ENDIAN_LITTLE 1
#define CM_ENDIAN_BIG 2

#define CM_ENDIAN CM_ENDIAN_LITTLE

/* Finds the compiler type and version.
*/
#if defined( __GCCE__ )
#   define CM_COMPILER CM_COMPILER_GCCE
#   define CM_COMP_VER _MSC_VER
//#	include <staticlibinit_gcce.h> // This is a GCCE toolchain workaround needed when compiling with GCCE 
#elif defined( __WINSCW__ )
#   define CM_COMPILER CM_COMPILER_WINSCW
#   define CM_COMP_VER _MSC_VER
#elif defined( _MSC_VER )
#   define CM_COMPILER CM_COMPILER_MSVC
#   define CM_COMP_VER _MSC_VER
#elif defined( __GNUC__ )
#   define CM_COMPILER CM_COMPILER_GNUC
#   define CM_COMP_VER (((__GNUC__)*100) + \
        (__GNUC_MINOR__*10) + \
        __GNUC_PATCHLEVEL__)

#elif defined( __BORLANDC__ )
#   define CM_COMPILER CM_COMPILER_BORL
#   define CM_COMP_VER __BCPLUSPLUS__
#   define __FUNCTION__ __FUNC__ 
#else
#   pragma error "No known compiler. Abort! Abort!"

#endif

/* See if we can use __forceinline or if we need to use __inline instead */
#if CM_COMPILER == CM_COMPILER_MSVC
#   if CM_COMP_VER >= 1200
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
#   define CM_PLATFORM CM_PLATFORM_WIN32
#elif defined( __APPLE_CC__)
#   define CM_PLATFORM CM_PLATFORM_APPLE
#else
#   define CM_PLATFORM CM_PLATFORM_LINUX
#endif

    /* Find the arch type */
#if defined(__x86_64__) || defined(_M_X64) || defined(__powerpc64__) || defined(__alpha__) || defined(__ia64__) || defined(__s390__) || defined(__s390x__)
#   define CM_ARCH_TYPE CM_ARCHITECTURE_64
#else
#   define CM_ARCH_TYPE CM_ARCHITECTURE_32
#endif

//----------------------------------------------------------------------------
// Windows Settings
#if CM_PLATFORM == CM_PLATFORM_WIN32

// If we're not including this from a client build, specify that the stuff
// should get exported. Otherwise, import it.
#	if defined( CM_STATIC_LIB )
		// Linux compilers don't have symbol import/export directives.
#   	define CM_UTILITY_EXPORT
#   else
#   	if defined( CM_UTILITY_EXPORTS )
#       	define CM_UTILITY_EXPORT __declspec( dllexport )
#   	else
#           if defined( __MINGW32__ )
#               define CM_UTILITY_EXPORT
#           else
#       	    define CM_UTILITY_EXPORT __declspec( dllimport )
#           endif
#   	endif
#	endif
// Win32 compilers use _DEBUG for specifying debug builds.
// for MinGW, we set DEBUG
#   if defined(_DEBUG) || defined(DEBUG)
#       define CM_DEBUG_MODE 1
#   else
#       define CM_DEBUG_MODE 0
#   endif

#endif // CM_PLATFORM == CM_PLATFORM_WIN32

//----------------------------------------------------------------------------
// Linux/Apple/Symbian Settings
#if CM_PLATFORM == CM_PLATFORM_LINUX || CM_PLATFORM == CM_PLATFORM_APPLE

// Enable GCC symbol visibility
#   if defined( CM_GCC_VISIBILITY )
#       define CM_UTILITY_EXPORT  __attribute__ ((visibility("default")))
#   else
#       define CM_UTILITY_EXPORT
#   endif

// A quick define to overcome different names for the same function
#   define stricmp strcasecmp

// Unlike the Win32 compilers, Linux compilers seem to use DEBUG for when
// specifying a debug build.
// (??? this is wrong, on Linux debug builds aren't marked in any way unless
// you mark it yourself any way you like it -- zap ???)
#   ifdef DEBUG
#       define CM_DEBUG_MODE 1
#   else
#       define CM_DEBUG_MODE 0
#   endif

#if CM_PLATFORM == CM_PLATFORM_APPLE
    #define CM_PLATFORM_LIB "CmPlatform.bundle"
#else //CM_PLATFORM_LINUX
    #define CM_PLATFORM_LIB "libCmPlatform.so"
#endif

#endif

//----------------------------------------------------------------------------

/* Initial CPU stuff to set.
*/
#define CM_CPU_UNKNOWN    0
#define CM_CPU_X86        1
#define CM_CPU_PPC        2

/* Find CPU type
*/
#if (defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))) || \
    (defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__)))
#   define CM_CPU CM_CPU_X86

#elif CM_PLATFORM == CM_PLATFORM_APPLE && defined(__BIG_ENDIAN__)
#   define CM_CPU CM_CPU_PPC
#elif CM_PLATFORM == CM_PLATFORM_APPLE
#	define CM_CPU CM_CPU_X86
#else
#   define CM_CPU CM_CPU_UNKNOWN
#endif

/* Find how to declare aligned variable.
*/
#if CM_COMPILER == CM_COMPILER_MSVC
#   define CM_ALIGNED_DECL(type, var, alignment)  __declspec(align(alignment)) type var

#elif CM_COMPILER == CM_COMPILER_GNUC
#   define CM_ALIGNED_DECL(type, var, alignment)  type var __attribute__((__aligned__(alignment)))

#else
#   define CM_ALIGNED_DECL(type, var, alignment)  type var
#endif

/** Find perfect alignment (should supports SIMD alignment if SIMD available)
*/
#if CM_CPU == CM_CPU_X86
#   define CM_SIMD_ALIGNMENT  16

#else
#   define CM_SIMD_ALIGNMENT  16
#endif

/* Declare variable aligned to SIMD alignment.
*/
#define CM_SIMD_ALIGNED_DECL(type, var)   CM_ALIGNED_DECL(type, var, CM_SIMD_ALIGNMENT)

}
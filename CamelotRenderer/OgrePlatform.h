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
#ifndef __Platform_H_
#define __Platform_H_

#include "OgreConfig.h"

namespace CamelotEngine {
/* Initial platform/compiler-related stuff to set.
*/
#define CM_PLATFORM_WIN32 1
#define CM_PLATFORM_LINUX 2
#define CM_PLATFORM_APPLE 3
#define OGRE_PLATFORM_SYMBIAN 4
#define OGRE_PLATFORM_IPHONE 5

#define CM_COMPILER_MSVC 1
#define CM_COMPILER_GNUC 2
#define CM_COMPILER_BORL 3
#define CM_COMPILER_WINSCW 4
#define CM_COMPILER_GCCE 5

#define OGRE_ENDIAN_LITTLE 1
#define OGRE_ENDIAN_BIG 2

#define CM_ARCHITECTURE_32 1
#define CM_ARCHITECTURE_64 2

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

#if defined( __SYMBIAN32__ ) 
#   define CM_PLATFORM OGRE_PLATFORM_SYMBIAN
#elif defined( __WIN32__ ) || defined( _WIN32 )
#   define CM_PLATFORM CM_PLATFORM_WIN32
#elif defined( __APPLE_CC__)
    // Device                                                     Simulator
    // Both requiring OS version 3.0 or greater
#   if __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ >= 30000 || __IPHONE_OS_VERSION_MIN_REQUIRED >= 30000
#       define CM_PLATFORM OGRE_PLATFORM_IPHONE
#   else
#       define CM_PLATFORM CM_PLATFORM_APPLE
#   endif
#else
#   define CM_PLATFORM CM_PLATFORM_LINUX
#endif

    /* Find the arch type */
#if defined(__x86_64__) || defined(_M_X64) || defined(__powerpc64__) || defined(__alpha__) || defined(__ia64__) || defined(__s390__) || defined(__s390x__)
#   define CM_ARCH_TYPE CM_ARCHITECTURE_64
#else
#   define CM_ARCH_TYPE CM_ARCHITECTURE_32
#endif

// For generating compiler warnings - should work on any compiler
// As a side note, if you start your message with 'Warning: ', the MSVC
// IDE actually does catch a warning :)
#define OGRE_QUOTE_INPLACE(x) # x
#define OGRE_QUOTE(x) OGRE_QUOTE_INPLACE(x)
#define OGRE_WARN( x )  message( __FILE__ "(" QUOTE( __LINE__ ) ") : " x "\n" )

//----------------------------------------------------------------------------
// Windows Settings
#if CM_PLATFORM == CM_PLATFORM_WIN32

// If we're not including this from a client build, specify that the stuff
// should get exported. Otherwise, import it.
#	if defined( CM_STATIC_LIB )
		// Linux compilers don't have symbol import/export directives.
#   	define CM_EXPORT
#   else
#   	if defined( CM_DLL )
#       	define CM_EXPORT __declspec( dllexport )
#   	else
#           if defined( __MINGW32__ )
#               define CM_EXPORT
#           else
#       	    define CM_EXPORT __declspec( dllimport )
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

// Disable unicode support on MingW for GCC 3, poorly supported in stdlibc++
// STLPORT fixes this though so allow if found
// MinGW C++ Toolkit supports unicode and sets the define __MINGW32_TOOLBOX_UNICODE__ in _mingw.h
// GCC 4 is also fine
#if defined(__MINGW32__)
# if CM_COMP_VER < 400
#  if !defined(_STLPORT_VERSION)
#   include<_mingw.h>
#   if defined(__MINGW32_TOOLBOX_UNICODE__) || CM_COMP_VER > 345
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
// Symbian Settings
#if CM_PLATFORM == OGRE_PLATFORM_SYMBIAN
#	define OGRE_UNICODE_SUPPORT 1
#   define CM_DEBUG_MODE 0
#   define CM_EXPORT
#	define CLOCKS_PER_SEC  1000
// pragma def were found here: http://www.inf.pucrs.br/~eduardob/disciplinas/SistEmbarcados/Mobile/Nokia/Tools/Carbide_vs/WINSCW/Help/PDF/C_Compilers_Reference_3.2.pdf
#	pragma warn_unusedarg off
#	pragma warn_emptydecl off
#	pragma warn_possunwant off
#endif
//----------------------------------------------------------------------------
// Linux/Apple/Symbian Settings
#if CM_PLATFORM == CM_PLATFORM_LINUX || CM_PLATFORM == CM_PLATFORM_APPLE || CM_PLATFORM == OGRE_PLATFORM_IPHONE || CM_PLATFORM == OGRE_PLATFORM_SYMBIAN

// Enable GCC symbol visibility
#   if defined( CM_GCC_VISIBILITY )
#       define CM_EXPORT  __attribute__ ((visibility("default")))
#   else
#       define CM_EXPORT
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
    #define CM_PLATFORM_LIB "OgrePlatform.bundle"
#elif CM_PLATFORM == OGRE_PLATFORM_IPHONE
    #define CM_PLATFORM_LIB "OgrePlatform.a"
#else //OGRE_PLATFORM_LINUX
    #define CM_PLATFORM_LIB "libOgrePlatform.so"
#endif

// Always enable unicode support for the moment
// Perhaps disable in old versions of gcc if necessary
#define OGRE_UNICODE_SUPPORT 1

#endif

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Endian Settings
// check for BIG_ENDIAN config flag, set OGRE_ENDIAN correctly
#ifdef OGRE_CONFIG_BIG_ENDIAN
#    define OGRE_ENDIAN OGRE_ENDIAN_BIG
#else
#    define OGRE_ENDIAN OGRE_ENDIAN_LITTLE
#endif

/* Initial CPU stuff to set.
*/
#define CM_CPU_UNKNOWN    0
#define CM_CPU_X86        1
#define CM_CPU_PPC        2
#define OGRE_CPU_ARM        3

/* Find CPU type
*/
#if (defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))) || \
    (defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__)))
#   define CM_CPU CM_CPU_X86

#elif CM_PLATFORM == CM_PLATFORM_APPLE && defined(__BIG_ENDIAN__)
#   define CM_CPU CM_CPU_PPC
#elif CM_PLATFORM == CM_PLATFORM_APPLE
#	define CM_CPU CM_CPU_X86
#elif CM_PLATFORM == OGRE_PLATFORM_IPHONE && (defined(__i386__) || defined(__x86_64__))
#	define CM_CPU CM_CPU_X86
#elif defined(__arm__)
#	define CM_CPU OGRE_CPU_ARM
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

/* Define whether or not Ogre compiled with SSE supports.
*/
#if OGRE_DOUBLE_PRECISION == 0 && CM_CPU == CM_CPU_X86 && CM_COMPILER == CM_COMPILER_MSVC
#   define __OGRE_HAVE_SSE  1
#elif OGRE_DOUBLE_PRECISION == 0 && CM_CPU == CM_CPU_X86 && CM_COMPILER == CM_COMPILER_GNUC && CM_PLATFORM != OGRE_PLATFORM_APPLE_IOS
#   define __OGRE_HAVE_SSE  1
#endif

/* Define whether or not Ogre compiled with VFP supports.
 */
#if OGRE_DOUBLE_PRECISION == 0 && CM_CPU == OGRE_CPU_ARM && CM_COMPILER == CM_COMPILER_GNUC && defined(__ARM_ARCH_6K__) && defined(__VFP_FP__)
#   define __OGRE_HAVE_VFP  1
#endif

/* Define whether or not Ogre compiled with NEON supports.
 */
#if OGRE_DOUBLE_PRECISION == 0 && CM_CPU == OGRE_CPU_ARM && CM_COMPILER == CM_COMPILER_GNUC && defined(__ARM_ARCH_7A__) && defined(__ARM_NEON__)
#   define __OGRE_HAVE_NEON  1
#endif

#ifndef __OGRE_HAVE_SSE
#   define __OGRE_HAVE_SSE  0
#endif

#ifndef __OGRE_HAVE_VFP
#   define __OGRE_HAVE_VFP  0
#endif

#ifndef __OGRE_HAVE_NEON
#   define __OGRE_HAVE_NEON  0
#endif

}

#endif

#pragma once

// Initial platform/compiler-related stuff to set.
#define CM_PLATFORM_WIN32 1
#define CM_PLATFORM_LINUX 2
#define CM_PLATFORM_APPLE 3

#define CM_COMPILER_MSVC 1
#define CM_COMPILER_GNUC 2
#define CM_COMPILER_INTEL 3
#define CM_COMPILER_CLANG 4

#define CM_ARCHITECTURE_x86_32 1
#define CM_ARCHITECTURE_x86_64 2

#define CM_ENDIAN_LITTLE 1
#define CM_ENDIAN_BIG 2

#define CM_ENDIAN CM_ENDIAN_LITTLE

// Finds the compiler type and version.
#if defined( _MSC_VER )
#   define CM_COMPILER CM_COMPILER_MSVC
#   define CM_COMP_VER _MSC_VER
#	define CM_THREADLOCAL __declspec(thread)
#elif defined( __GNUC__ )
#   define CM_COMPILER CM_COMPILER_GNUC
#   define CM_COMP_VER (((__GNUC__)*100) + \
        (__GNUC_MINOR__*10) + \
        __GNUC_PATCHLEVEL__)
#   define CM_THREADLOCAL __thread
#elif defined ( __INTEL_COMPILER )
#   define CM_COMPILER CM_COMPILER_INTEL
#	define CM_COMP_VER __INTEL_COMPILER
	// CM_THREADLOCAL define is down below because Intel compiler defines it differently based on platform
#elif defined ( __clang__ )
#   define CM_COMPILER CM_COMPILER_CLANG
#	define CM_COMP_VER __clang_major__
#   define CM_THREADLOCAL __thread
#else
#   pragma error "No known compiler. "

#endif

// See if we can use __forceinline or if we need to use __inline instead
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

// Finds the current platform
#if defined( __WIN32__ ) || defined( _WIN32 )
#   define CM_PLATFORM CM_PLATFORM_WIN32
#elif defined( __APPLE_CC__)
#   define CM_PLATFORM CM_PLATFORM_APPLE
#else
#   define CM_PLATFORM CM_PLATFORM_LINUX
#endif

// Find the architecture type
#if defined(__x86_64__) || defined(_M_X64)
#   define CM_ARCH_TYPE CM_ARCHITECTURE_x86_64
#else
#   define CM_ARCH_TYPE CM_ARCHITECTURE_x86_32
#endif

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

#	if CM_COMPILER == CM_COMPILER_INTEL
#        define CM_THREADLOCAL __declspec(thread)
#	endif

#endif // CM_PLATFORM == CM_PLATFORM_WIN32

// Linux/Apple Settings
#if CM_PLATFORM == CM_PLATFORM_LINUX || CM_PLATFORM == CM_PLATFORM_APPLE

// Enable GCC symbol visibility
#   if defined( CM_GCC_VISIBILITY )
#       define CM_UTILITY_EXPORT  __attribute__ ((visibility("default")))
#   else
#       define CM_UTILITY_EXPORT
#   endif

#   ifdef DEBUG
#       define CM_DEBUG_MODE 1
#   else
#       define CM_DEBUG_MODE 0
#   endif

#	if CM_COMPILER == CM_COMPILER_INTEL
#        define CM_THREADLOCAL __thread
#	endif

#if CM_PLATFORM == CM_PLATFORM_APPLE
    #define CM_PLATFORM_LIB "CmPlatform.bundle"
#else //CM_PLATFORM_LINUX
    #define CM_PLATFORM_LIB "libCmPlatform.so"
#endif

#endif
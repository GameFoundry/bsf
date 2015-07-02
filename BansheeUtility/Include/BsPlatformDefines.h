#pragma once

// Initial platform/compiler-related stuff to set.
#define BS_PLATFORM_WIN32 1
#define BS_PLATFORM_LINUX 2
#define BS_PLATFORM_APPLE 3

#define BS_COMPILER_MSVC 1
#define BS_COMPILER_GNUC 2
#define BS_COMPILER_INTEL 3
#define BS_COMPILER_CLANG 4

#define BS_ARCHITECTURE_x86_32 1
#define BS_ARCHITECTURE_x86_64 2

#define BS_ENDIAN_LITTLE 1
#define BS_ENDIAN_BIG 2
#define BS_ENDIAN BS_ENDIAN_LITTLE

#define BS_EDITOR_BUILD 1

// Finds the compiler type and version.
#if defined( _MSC_VER )
#   define BS_COMPILER BS_COMPILER_MSVC
#   define BS_COMP_VER _MSC_VER
#	define BS_THREADLOCAL __declspec(thread)
#elif defined( __GNUC__ )
#   define BS_COMPILER BS_COMPILER_GNUC
#   define BS_COMP_VER (((__GNUC__)*100) + \
        (__GNUC_MINOR__*10) + \
        __GNUC_PATCHLEVEL__)
#   define BS_THREADLOCAL __thread
#elif defined ( __INTEL_COMPILER )
#   define BS_COMPILER BS_COMPILER_INTEL
#	define BS_COMP_VER __INTEL_COMPILER
	// BS_THREADLOCAL define is down below because Intel compiler defines it differently based on platform
#elif defined ( __clang__ )
#   define BS_COMPILER BS_COMPILER_CLANG
#	define BS_COMP_VER __clang_major__
#   define BS_THREADLOCAL __thread
#else
#   pragma error "No known compiler. "

#endif

// See if we can use __forceinline or if we need to use __inline instead
#if BS_COMPILER == BS_COMPILER_MSVC
#   if BS_COMP_VER >= 1200
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
#   define BS_PLATFORM BS_PLATFORM_WIN32
#elif defined( __APPLE_CC__)
#   define BS_PLATFORM BS_PLATFORM_APPLE
#else
#   define BS_PLATFORM BS_PLATFORM_LINUX
#endif

// Find the architecture type
#if defined(__x86_64__) || defined(_M_X64)
#   define BS_ARCH_TYPE BS_ARCHITECTURE_x86_64
#else
#   define BS_ARCH_TYPE BS_ARCHITECTURE_x86_32
#endif

// Windows Settings
#if BS_PLATFORM == BS_PLATFORM_WIN32

// If we're not including this from a client build, specify that the stuff
// should get exported. Otherwise, import it.
#	if defined( BS_STATIC_LIB )
		// Linux compilers don't have symbol import/export directives.
#   	define BS_UTILITY_EXPORT
#   else
#   	if defined(BS_UTILITY_EXPORTS)
#       	define BS_UTILITY_EXPORT __declspec( dllexport )
#   	else
#           if defined( __MINGW32__ )
#               define BS_UTILITY_EXPORT
#           else
#       	    define BS_UTILITY_EXPORT __declspec( dllimport )
#           endif
#   	endif
#	endif
// Win32 compilers use _DEBUG for specifying debug builds.
// for MinGW, we set DEBUG
#   if defined(_DEBUG) || defined(DEBUG)
#       define BS_DEBUG_MODE 1
#   else
#       define BS_DEBUG_MODE 0
#   endif

#	if BS_COMPILER == BS_COMPILER_INTEL
#        define BS_THREADLOCAL __declspec(thread)
#	endif

#endif // BS_PLATFORM == BS_PLATFORM_WIN32

// Linux/Apple Settings
#if BS_PLATFORM == BS_PLATFORM_LINUX || BS_PLATFORM == BS_PLATFORM_APPLE

// Enable GCC symbol visibility
#   if defined( BS_GCC_VISIBILITY )
#       define BS_UTILITY_EXPORT  __attribute__ ((visibility("default")))
#   else
#       define BS_UTILITY_EXPORT
#   endif

#   ifdef DEBUG
#       define BS_DEBUG_MODE 1
#   else
#       define BS_DEBUG_MODE 0
#   endif

#	if BS_COMPILER == BS_COMPILER_INTEL
#        define BS_THREADLOCAL __thread
#	endif

#endif
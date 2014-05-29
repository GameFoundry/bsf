#pragma once

#include "BsPrerequisites.h"

#if (BS_PLATFORM == BS_PLATFORM_WIN32) && !defined(__MINGW32__)
#	ifdef BS_MONO_EXPORTS
#		define BS_MONO_EXPORT __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define BS_MONO_EXPORT
#       else
#    		define BS_MONO_EXPORT __declspec(dllimport)
#       endif
#	endif
#elif defined ( BS_GCC_VISIBILITY )
#    define BS_MONO_EXPORT  __attribute__ ((visibility("default")))
#else
#    define BS_MONO_EXPORT
#endif

namespace BansheeEngine
{
	class MonoManager;
	class MonoAssembly;
	class MonoClass;
	class MonoMethod;
	class MonoField;
	class MonoProperty;
}
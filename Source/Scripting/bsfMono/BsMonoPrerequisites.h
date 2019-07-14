//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsPrerequisites.h"

// DLL export
#if BS_PLATFORM == BS_PLATFORM_WIN32 // Windows
#  if BS_COMPILER == BS_COMPILER_MSVC
#    if defined(BS_MONO_STATIC_LIB)
#      define BS_MONO_EXPORT
#    else
#      if defined(BS_MONO_EXPORTS)
#        define BS_MONO_EXPORT __declspec(dllexport)
#      else
#        define BS_MONO_EXPORT __declspec(dllimport)
#      endif
#	 endif
#  else
#    if defined(BS_MONO_STATIC_LIB)
#      define BS_MONO_EXPORT
#    else
#      if defined(BS_MONO_EXPORTS)
#        define BS_MONO_EXPORT __attribute__ ((dllexport))
#      else
#        define BS_MONO_EXPORT __attribute__ ((dllimport))
#      endif
#	 endif
#  endif
#  define BS_MONO_HIDDEN
#else // Linux/Mac settings
#  define BS_MONO_EXPORT __attribute__ ((visibility ("default")))
#  define BS_MONO_HIDDEN __attribute__ ((visibility ("hidden")))
#endif

/** @addtogroup Plugins
 *  @{
 */

/** @defgroup Mono bsfMono
 *	Scripting language backend using Mono's CLR.
 */

/** @} */

namespace bs
{
	class MonoManager;
	class MonoAssembly;
	class MonoClass;
	class MonoMethod;
	class MonoField;
	class MonoProperty;

	/** A list of all valid Mono primitive types. */
	enum class MonoPrimitiveType
	{
		Boolean,
		Char,
		I8,
		U8,
		I16,
		U16,
		I32,
		U32,
		I64,
		U64,
		R32,
		R64,
		String,
		ValueType,
		Class,
		Array,
		Generic,
		Unknown
	};

	/**	Returns the level of member visibility in the class. */
	enum class MonoMemberVisibility // Note: Elements must be ordered from most to least hidden
	{
		Private,
		Protected,
		Internal,
		ProtectedInternal,
		Public
	};
}

#if BS_PLATFORM == BS_PLATFORM_WIN32
	#define BS_THUNKCALL BS_STDCALL
#else
	#define BS_THUNKCALL
#endif

typedef struct _MonoClass MonoClass;
typedef struct _MonoDomain MonoDomain;
typedef struct _MonoImage MonoImage;
typedef struct _MonoAssembly MonoAssembly;
typedef struct _MonoMethod MonoMethod;
typedef struct _MonoProperty MonoProperty;
typedef struct _MonoObject MonoObject;
typedef struct _MonoString MonoString;
typedef struct _MonoArray MonoArray;
typedef struct _MonoReflectionType MonoReflectionType;
typedef struct _MonoException MonoException;
typedef struct _MonoClassField MonoClassField;

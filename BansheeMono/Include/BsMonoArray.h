//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsMonoPrerequisites.h"
#include "BsScriptMeta.h"
#include "BsMonoManager.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	/** Helper class for creating and parsing managed arrays.*/
	class BS_MONO_EXPORT ScriptArray
	{
	public:
		/** Wraps an existing array and allows you to get/set its values. */
		ScriptArray(MonoArray* existingArray);
		/** Creates a new array of specified size with elements of the specified type. */
		ScriptArray(MonoClass& klass, UINT32 size);
		/** Creates a new array of specified size with elements of the specified type. */
		ScriptArray(::MonoClass* klass, UINT32 size);

		/** Retrieves an entry from the array at the specified index. */
		template<class T>
		T get(UINT32 idx)
		{
			return mono_array_get(mInternal, T, idx);
		}

		/** Sets an entry from the array at the specified index. */
		template<class T>
		void set(UINT32 idx, const T& value)
		{
			mono_array_set(mInternal, T, idx, value);
		}

		/** Retrieves an entry as a native string from the array at the specified index. */
		template<>
		String get(UINT32 idx);

		/** Retrieves an entry as a native string from the array at the specified index. */
		template<>
		WString get(UINT32 idx);

		/** Sets a native string entry from the array at the specified index. */
		template<>
		void set<String>(UINT32 idx, const String& value);

		/** Sets a native string entry from the array at the specified index. */
		template<>
		void set<WString>(UINT32 idx, const WString& value);

		/** Returns the raw object from the array at the specified index. */
		template<class T>
		T* getRawPtr(UINT32 idx = 0)
		{
#if BS_DEBUG_MODE
			assert(sizeof(T) == elementSize());
#endif

			return (T*)mono_array_addr(mInternal, T, idx);
		}

		/** 
		 * Returns the raw object from the array at the specified index. Provided size determines the size of each
		 * element in the array. Caller must ensure it is correct for the specified array.
		 */
		UINT8* getRawPtr(UINT32 size, UINT32 idx = 0)
		{
#if BS_DEBUG_MODE
			assert(size == elementSize());
#endif

			return (UINT8*)mono_array_addr_with_size(mInternal, size, idx);
		}

		/** 
		 * Creates a new array of managed objects. 
		 *
		 * @tparam	T	ScriptObject wrapper for the specified managed type.
		 */
		template<class T>
		static ScriptArray create(UINT32 size)
		{
			return ScriptArray(*T::getMetaData()->scriptClass, size);
		}

		/** Creates a new array of integers. */
		template<>
		static ScriptArray create<UINT32>(UINT32 size)
		{
			return ScriptArray(mono_get_uint32_class(), size);
		}

		/** Creates a new array of integers. */
		template<>
		static ScriptArray create<INT32>(UINT32 size)
		{
			return ScriptArray(mono_get_int32_class(), size);
		}

		/** Creates a new array of integers. */
		template<>
		static ScriptArray create<UINT64>(UINT32 size)
		{
			return ScriptArray(mono_get_uint64_class(), size);
		}

		/** Creates a new array of integers. */
		template<>
		static ScriptArray create<INT64>(UINT32 size)
		{
			return ScriptArray(mono_get_int64_class(), size);
		}

		/** Creates a new array of strings. */
		template<>
		static ScriptArray create<WString>(UINT32 size)
		{
			return ScriptArray(mono_get_string_class(), size);
		}

		/** Creates a new array of strings. */
		template<>
		static ScriptArray create<String>(UINT32 size)
		{
			return ScriptArray(mono_get_string_class(), size);
		}

		/** Creates a new array of floats. */
		template<>
		static ScriptArray create<float>(UINT32 size)
		{
			return ScriptArray(mono_get_single_class(), size);
		}

		/** Creates a new array of booleans. */
		template<>
		static ScriptArray create<bool>(UINT32 size)
		{
			return ScriptArray(mono_get_boolean_class(), size);
		}

		/** Returns number of elements in the array. */
		UINT32 size() const;

		/** Returns the size of an individual element in the array, in bytes. */
		UINT32 elementSize() const;

		/** Returns the managed object representing this array. */
		MonoArray* getInternal() const { return mInternal; }

	private:
		MonoArray* mInternal;
	};
}
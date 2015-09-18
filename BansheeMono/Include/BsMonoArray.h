#pragma once

#include "BsMonoPrerequisites.h"
#include "BsScriptMeta.h"
#include "BsMonoManager.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_MONO_EXPORT ScriptArray
	{
	public:
		ScriptArray(MonoArray* existingArray);
		ScriptArray(MonoClass& klass, UINT32 size);
		ScriptArray(::MonoClass* klass, UINT32 size);

		template<class T>
		T get(UINT32 idx)
		{
			return mono_array_get(mInternal, T, idx);
		}

		template<class T>
		void set(UINT32 idx, const T& value)
		{
			mono_array_set(mInternal, T, idx, value);
		}

		template<>
		String get(UINT32 idx);

		template<>
		WString get(UINT32 idx);

		template<>
		void set<String>(UINT32 idx, const String& value);

		template<>
		void set<WString>(UINT32 idx, const WString& value);

		template<class T>
		T* getRawPtr(UINT32 idx = 0)
		{
#if BS_DEBUG_MODE
			assert(sizeof(T) == elementSize());
#endif

			return (T*)mono_array_addr(mInternal, T, idx);
		}

		UINT8* getRawPtr(UINT32 size, UINT32 idx = 0)
		{
#if BS_DEBUG_MODE
			assert(size == elementSize());
#endif

			return (UINT8*)mono_array_addr_with_size(mInternal, size, idx);
		}

		template<class T>
		static ScriptArray create(UINT32 size)
		{
			return ScriptArray(*T::getMetaData()->scriptClass, size);
		}

		template<>
		static ScriptArray create<UINT32>(UINT32 size)
		{
			return ScriptArray(mono_get_uint32_class(), size);
		}

		template<>
		static ScriptArray create<INT32>(UINT32 size)
		{
			return ScriptArray(mono_get_int32_class(), size);
		}

		template<>
		static ScriptArray create<UINT64>(UINT32 size)
		{
			return ScriptArray(mono_get_uint64_class(), size);
		}

		template<>
		static ScriptArray create<INT64>(UINT32 size)
		{
			return ScriptArray(mono_get_int64_class(), size);
		}

		template<>
		static ScriptArray create<WString>(UINT32 size)
		{
			return ScriptArray(mono_get_string_class(), size);
		}

		template<>
		static ScriptArray create<String>(UINT32 size)
		{
			return ScriptArray(mono_get_string_class(), size);
		}

		template<>
		static ScriptArray create<float>(UINT32 size)
		{
			return ScriptArray(mono_get_single_class(), size);
		}

		template<>
		static ScriptArray create<bool>(UINT32 size)
		{
			return ScriptArray(mono_get_boolean_class(), size);
		}

		UINT32 size() const;
		UINT32 elementSize() const;

		MonoArray* getInternal() const { return mInternal; }

	private:
		MonoArray* mInternal;
	};
}
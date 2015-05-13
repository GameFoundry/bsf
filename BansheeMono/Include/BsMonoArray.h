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

		template<class T>
		T* getRawPtr(UINT32 offset = 0)
		{
#if BS_DEBUG_MODE
			int nativeSize = sizeof(T);

			::MonoClass* arrayClass = mono_object_get_class((MonoObject*)(mInternal));
			::MonoClass* elementClass = mono_class_get_element_class(arrayClass);

			int monoSize = mono_class_array_element_size(elementClass);
			assert(nativeSize == monoSize);
#endif

			return (T*)mono_array_addr(mInternal, T, offset);
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

		UINT32 size() const;

		MonoArray* getInternal() const { return mInternal; }

	private:
		MonoArray* mInternal;
	};
}
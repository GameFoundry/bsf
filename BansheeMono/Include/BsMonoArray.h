#pragma once

#include "BsMonoPrerequisites.h"
#include "BsScriptMeta.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_MONO_EXPORT ScriptArray
	{
	public:
		ScriptArray(MonoArray* existingArray);
		ScriptArray(MonoClass& klass, UINT32 size);

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
		static ScriptArray create(UINT32 size)
		{
			return ScriptArray(*T::getMetaData()->scriptClass, size);
		}

		UINT32 size() const;

		MonoArray* getInternal() const { return mInternal; }

	private:
		MonoArray* createArray(MonoClass& klass, UINT32 size);

		MonoArray* mInternal;
	};
}
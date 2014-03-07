#pragma once

#include "BsMonoPrerequisites.h"
#include <mono/jit/jit.h>
#include <mono/metadata/class.h>

namespace BansheeEngine
{
	class BS_MONO_EXPORT MonoField
	{
	public:
		void* getValue(MonoObject* instance);
		void setValue(MonoObject* instance, void* value);

		bool hasAttribute(MonoClass* monoClass);
		MonoObject* getAttribute(MonoClass* monoClass);

	private:
		friend class MonoClass;

		MonoField(MonoClassField* field);

		MonoClassField* mField;
		void* mThunk;
	};
}
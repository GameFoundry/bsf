#pragma once

#include "BsPrerequisites.h"
#include <mono/jit/jit.h>
#include <mono/metadata/class.h>

namespace BansheeEngine
{
	class BS_EXPORT ScriptField
	{
	public:
		void* getValue(MonoObject* instance);
		void setValue(MonoObject* instance, void* value);

	private:
		friend class ScriptClass;

		ScriptField(MonoClassField* field);

		MonoClassField* mField;
		void* mThunk;
	};
}
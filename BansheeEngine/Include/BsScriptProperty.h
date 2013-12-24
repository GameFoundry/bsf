#pragma once

#include "BsPrerequisites.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_EXPORT ScriptProperty
	{
	public:
		MonoObject* get(MonoObject* instance) const;
		void set(MonoObject* instance, MonoObject* value) const;

	private:
		friend class ScriptClass;

		ScriptProperty(MonoProperty* monoProp);

		MonoProperty* mProperty;
		MonoMethod* mGetMethod;
		MonoMethod* mSetMethod;
	};
}
#pragma once

#include "BsMonoPrerequisites.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_MONO_EXPORT MonoProperty
	{
	public:
		MonoObject* get(MonoObject* instance) const;
		void set(MonoObject* instance, void* value) const;

		MonoObject* getIndexed(MonoObject* instance, void* index) const;
		void setIndexed(MonoObject* instance, void* index, void* value) const;

		MonoClass* getReturnType();
	private:
		friend class MonoClass;

		MonoProperty(::MonoProperty* monoProp);

		::MonoProperty* mProperty;
		::MonoMethod* mGetMethod;
		::MonoMethod* mSetMethod;
		MonoClass* mGetReturnType;
	};
}
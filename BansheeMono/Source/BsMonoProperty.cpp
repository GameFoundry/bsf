#include "BsMonoProperty.h"
#include "BsMonoMethod.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	MonoProperty::MonoProperty(::MonoProperty* monoProp)
		:mProperty(monoProp)
	{
		mGetMethod = mono_property_get_get_method(mProperty);
		mSetMethod = mono_property_get_set_method(mProperty);
	}

	MonoObject* MonoProperty::get(MonoObject* instance) const
	{
		return mono_runtime_invoke(mGetMethod, instance, nullptr, nullptr);
	}

	void MonoProperty::set(MonoObject* instance, MonoObject* value) const
	{
		void* args[1];
		args[0] = value;
		mono_runtime_invoke(mSetMethod, instance, args, nullptr);
	}	
}
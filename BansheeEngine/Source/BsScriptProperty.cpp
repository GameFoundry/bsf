#include "BsScriptProperty.h"
#include "BsScriptObject.h"
#include "BsScriptMethod.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptProperty::ScriptProperty(MonoProperty* monoProp)
		:mProperty(monoProp)
	{
		mGetMethod = mono_property_get_get_method(mProperty);
		mSetMethod = mono_property_get_set_method(mProperty);
	}

	MonoObject* ScriptProperty::get(MonoObject* instance) const
	{
		return mono_runtime_invoke(mGetMethod, instance, nullptr, nullptr);
	}

	void ScriptProperty::set(MonoObject* instance, MonoObject* value) const
	{
		void* args[1];
		args[0] = value;
		mono_runtime_invoke(mSetMethod, instance, args, nullptr);
	}	
}
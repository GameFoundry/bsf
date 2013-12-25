#include "BsScriptField.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptField::ScriptField(MonoClassField* field)
		:mField(field)
	{

	}

	void* ScriptField::getValue(MonoObject* instance)
	{
		void* value = nullptr;
		mono_field_get_value(instance, mField, &value);
		return value;
	}

	void ScriptField::setValue(MonoObject* instance, void* value)
	{
		mono_field_set_value(instance, mField, &value);
	}
}
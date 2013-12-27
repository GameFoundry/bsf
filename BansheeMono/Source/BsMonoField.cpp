#include "BsMonoField.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	MonoField::MonoField(MonoClassField* field)
		:mField(field)
	{

	}

	void* MonoField::getValue(MonoObject* instance)
	{
		void* value = nullptr;
		mono_field_get_value(instance, mField, &value);
		return value;
	}

	void MonoField::setValue(MonoObject* instance, void* value)
	{
		mono_field_set_value(instance, mField, &value);
	}
}
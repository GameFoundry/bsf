#include "BsMonoField.h"
#include "BsMonoClass.h"

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

	bool MonoField::hasAttribute(MonoClass* monoClass)
	{
		// TODO - Consider caching custom attributes or just initializing them all at load

		::MonoClass* parentClass = mono_field_get_parent(mField);
		MonoCustomAttrInfo* attrInfo = mono_custom_attrs_from_field(parentClass, mField);

		bool hasAttr = mono_custom_attrs_has_attr(attrInfo, monoClass->_getInternalClass());

		mono_custom_attrs_free(attrInfo);

		return hasAttr;
	}

	MonoObject* MonoField::getAttribute(MonoClass* monoClass)
	{
		// TODO - Consider caching custom attributes or just initializing them all at load

		::MonoClass* parentClass = mono_field_get_parent(mField);
		MonoCustomAttrInfo* attrInfo = mono_custom_attrs_from_field(parentClass, mField);

		MonoObject* foundAttr = mono_custom_attrs_get_attr(attrInfo, monoClass->_getInternalClass());

		mono_custom_attrs_free(attrInfo);

		return foundAttr;
	}
}
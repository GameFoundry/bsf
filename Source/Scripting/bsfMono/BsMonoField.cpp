//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"

#include <mono/metadata/attrdefs.h>
#include <mono/jit/jit.h>
#include <mono/metadata/class.h>

namespace bs
{
	MonoField::MonoField(MonoClassField* field)
		:mField(field), mFieldType(nullptr)
	{
		mName = mono_field_get_name(mField);
	}

	MonoClass* MonoField::getType()
	{
		if(mFieldType != nullptr)
			return mFieldType;

		MonoType* monoType = mono_field_get_type(mField);
		::MonoClass* fieldClass = mono_class_from_mono_type(monoType);
		if(fieldClass == nullptr)
			return nullptr;	
		
		mFieldType = MonoManager::instance().findClass(fieldClass);

		return mFieldType;
	}

	void MonoField::get(MonoObject* instance, void* outValue)
	{
		mono_field_get_value(instance, mField, outValue);
	}

	MonoObject* MonoField::getBoxed(MonoObject* instance)
	{
		return mono_field_get_value_object(MonoManager::instance().getDomain(), mField, instance);
	}

	void MonoField::set(MonoObject* instance, void* value)
	{
		mono_field_set_value(instance, mField, value);
	}

	bool MonoField::hasAttribute(MonoClass* monoClass)
	{
		// TODO - Consider caching custom attributes or just initializing them all at load

		::MonoClass* parentClass = mono_field_get_parent(mField);
		MonoCustomAttrInfo* attrInfo = mono_custom_attrs_from_field(parentClass, mField);
		if(attrInfo == nullptr)
			return false;

		bool hasAttr = mono_custom_attrs_has_attr(attrInfo, monoClass->_getInternalClass()) != 0;
		
		mono_custom_attrs_free(attrInfo);

		return hasAttr;
	}

	MonoObject* MonoField::getAttribute(MonoClass* monoClass)
	{
		// TODO - Consider caching custom attributes or just initializing them all at load

		::MonoClass* parentClass = mono_field_get_parent(mField);
		MonoCustomAttrInfo* attrInfo = mono_custom_attrs_from_field(parentClass, mField);
		if(attrInfo == nullptr)
			return nullptr;

		MonoObject* foundAttr = nullptr;
		if(mono_custom_attrs_has_attr(attrInfo, monoClass->_getInternalClass()))
			foundAttr = mono_custom_attrs_get_attr(attrInfo, monoClass->_getInternalClass());

		mono_custom_attrs_free(attrInfo);
		return foundAttr;
	}

	MonoMemberVisibility MonoField::getVisibility()
	{
		uint32_t flags = mono_field_get_flags(mField) & MONO_FIELD_ATTR_FIELD_ACCESS_MASK;

		if(flags == MONO_FIELD_ATTR_PRIVATE)
			return MonoMemberVisibility::Private;
		else if(flags == MONO_FIELD_ATTR_FAM_AND_ASSEM)
			return MonoMemberVisibility::ProtectedInternal;
		else if(flags == MONO_FIELD_ATTR_ASSEMBLY)
			return MonoMemberVisibility::Internal;
		else if(flags == MONO_FIELD_ATTR_FAMILY)
			return MonoMemberVisibility::Protected;
		else if(flags == MONO_FIELD_ATTR_PUBLIC)
			return MonoMemberVisibility::Public;

		assert(false);
		return MonoMemberVisibility::Private;
	}

	bool MonoField::isStatic()
	{
		uint32_t flags = mono_field_get_flags(mField);

		return (flags & MONO_FIELD_ATTR_STATIC) != 0;
	}
}

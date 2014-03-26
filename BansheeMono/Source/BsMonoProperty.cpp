#include "BsMonoProperty.h"
#include "BsMonoMethod.h"
#include "BsMonoManager.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	MonoProperty::MonoProperty(::MonoProperty* monoProp)
		:mProperty(monoProp), mPropertyType(nullptr)
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

	MonoClass* MonoProperty::getReturnType()
	{
		if(mPropertyType != nullptr)
			return mPropertyType;

		MonoType* returnType = mono_signature_get_return_type(mono_method_signature(mGetMethod));
		if(returnType == nullptr)
			return nullptr;

		::MonoClass* returnClass = mono_class_from_mono_type(returnType);
		if(returnClass == nullptr)
			return nullptr;	

		mPropertyType = MonoManager::instance().findClass(returnClass);
		return mPropertyType;
	}
}
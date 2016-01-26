//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsMonoProperty.h"
#include "BsMonoMethod.h"
#include "BsMonoManager.h"

namespace BansheeEngine
{
	MonoProperty::MonoProperty(::MonoProperty* monoProp)
		:mProperty(monoProp), mGetReturnType(nullptr)
	{
		mGetMethod = mono_property_get_get_method(mProperty);
		mSetMethod = mono_property_get_set_method(mProperty);
	}

	MonoObject* MonoProperty::get(MonoObject* instance) const
	{
		return mono_runtime_invoke(mGetMethod, instance, nullptr, nullptr);
	}

	void MonoProperty::set(MonoObject* instance, void* value) const
	{
		void* args[1];
		args[0] = value;
		mono_runtime_invoke(mSetMethod, instance, args, nullptr);
	}	

	MonoObject* MonoProperty::getIndexed(MonoObject* instance, UINT32 index) const
	{
		void* args[1];
		args[0] = &index;
		return mono_runtime_invoke(mGetMethod, instance, args, nullptr);
	}

	void MonoProperty::setIndexed(MonoObject* instance, UINT32 index, void* value) const
	{
		void* args[2];
		args[0] = &index;
		args[1] = value;
		mono_runtime_invoke(mSetMethod, instance, args, nullptr);
	}	

	MonoClass* MonoProperty::getReturnType()
	{
		if(mGetReturnType != nullptr)
			return mGetReturnType;

		MonoType* returnType = mono_signature_get_return_type(mono_method_signature(mGetMethod));
		if(returnType == nullptr)
			return nullptr;

		::MonoClass* returnClass = mono_class_from_mono_type(returnType);
		if(returnClass == nullptr)
			return nullptr;	

		mGetReturnType = MonoManager::instance().findClass(returnClass);
		return mGetReturnType;
	}
}
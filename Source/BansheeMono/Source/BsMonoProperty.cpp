//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsMonoProperty.h"
#include "BsMonoMethod.h"
#include "BsMonoManager.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	MonoProperty::MonoProperty(::MonoProperty* monoProp)
		:mProperty(monoProp), mGetReturnType(nullptr), mIsIndexed(false), mIsFullyInitialized(false)
	{
		mGetMethod = mono_property_get_get_method(mProperty);
		mSetMethod = mono_property_get_set_method(mProperty);
	}

	MonoObject* MonoProperty::get(MonoObject* instance) const
	{
		if (mGetMethod == nullptr)
			return nullptr;

		return mono_runtime_invoke(mGetMethod, instance, nullptr, nullptr);
	}

	void MonoProperty::set(MonoObject* instance, void* value) const
	{
		if (mSetMethod == nullptr)
			return;

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

	bool MonoProperty::isIndexed() const
	{
		if (!mIsFullyInitialized)
			initializeDeferred();

		return mIsIndexed;
	}

	MonoClass* MonoProperty::getReturnType() const
	{
		if (!mIsFullyInitialized)
			initializeDeferred();

		return mGetReturnType;
	}

	void MonoProperty::initializeDeferred() const
	{
		if (mGetMethod != nullptr)
		{
			MonoMethodSignature* signature = mono_method_signature(mGetMethod);

			MonoType* returnType = mono_signature_get_return_type(signature);
			if (returnType != nullptr)
			{
				::MonoClass* returnClass = mono_class_from_mono_type(returnType);
				if (returnClass != nullptr)
					mGetReturnType = MonoManager::instance().findClass(returnClass);
			}

			UINT32 numParams = mono_signature_get_param_count(signature);
			mIsIndexed = numParams == 1;
		}
		else if(mSetMethod != nullptr)
		{
			MonoMethodSignature* signature = mono_method_signature(mSetMethod);

			UINT32 numParams = mono_signature_get_param_count(signature);
			mIsIndexed = numParams == 2;
		}

		mIsFullyInitialized = true;
	}
}
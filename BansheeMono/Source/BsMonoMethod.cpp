#include "BsMonoMethod.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"

namespace BansheeEngine
{
	MonoMethod::MonoMethod(::MonoMethod* method)
		:mMethod(method), mReturnType(nullptr)
	{
		mThunk = mono_method_get_unmanaged_thunk(mMethod);
	}

	MonoObject* MonoMethod::invoke(MonoObject* instance, void** params)
	{
		MonoObject* exception = nullptr;
		MonoObject* retVal = mono_runtime_invoke(mMethod, instance, params, &exception);

		MonoUtil::throwIfException(exception);
		return retVal;
	}		

	MonoObject* MonoMethod::invokeVirtual(MonoObject* instance, void** params)
	{
		::MonoMethod* virtualMethod = mono_object_get_virtual_method(instance, mMethod);

		MonoObject* exception = nullptr;
		MonoObject* retVal = mono_runtime_invoke(virtualMethod, instance, params, nullptr);

		MonoUtil::throwIfException(exception);
		return retVal;
	}		

	void* MonoMethod::getThunk() const
	{
		return mThunk;
	}

	String MonoMethod::getName() const
	{
		return String(mono_method_get_name(mMethod));
	}

	MonoClass* MonoMethod::getReturnType()
	{
		if(mReturnType != nullptr)
			return mReturnType;

		MonoType* returnType = mono_signature_get_return_type(mono_method_signature(mMethod));
		if(returnType == nullptr)
			return nullptr;

		::MonoClass* returnClass = mono_class_from_mono_type(returnType);
		if(returnClass == nullptr)
			return nullptr;	

		mReturnType = MonoManager::instance().findClass(returnClass);
		return mReturnType;
	}
}
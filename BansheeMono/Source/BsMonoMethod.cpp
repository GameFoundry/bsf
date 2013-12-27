#include "BsMonoMethod.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	MonoMethod::MonoMethod(::MonoMethod* method)
		:mMethod(method)
	{
		mThunk = mono_method_get_unmanaged_thunk(mMethod);
	}

	MonoObject* MonoMethod::invoke(MonoObject* instance, void** params)
	{
		return mono_runtime_invoke(mMethod, instance, params, nullptr);
	}		

	void* MonoMethod::getThunk() const
	{
		return mThunk;
	}

	String MonoMethod::getName() const
	{
		return String(mono_method_get_name(mMethod));
	}
}
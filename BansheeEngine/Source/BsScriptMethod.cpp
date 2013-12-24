#include "BsScriptMethod.h"
#include "BsScriptObject.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptMethod::ScriptMethod(MonoMethod* method)
		:mMethod(method)
	{
		mThunk = mono_method_get_unmanaged_thunk(mMethod);
	}

	MonoObject* ScriptMethod::invoke(MonoObject* instance, void** params)
	{
		return mono_runtime_invoke(mMethod, instance, params, nullptr);
	}		

	void* ScriptMethod::getThunk() const
	{
		return mThunk;
	}

	String ScriptMethod::getName() const
	{
		return String(mono_method_get_name(mMethod));
	}
}
#include "CmScriptMethod.h"
#include "CmScriptObject.h"

namespace CamelotFramework
{
	ScriptMethod::ScriptMethod(MonoMethod* method)
		:mMethod(method)
	{
		mThunk = mono_method_get_unmanaged_thunk(mMethod);
	}

	MonoObject* ScriptMethod::invoke(ScriptObject* instance, void** params)
	{
		return mono_runtime_invoke(mMethod, instance->getMonoObject(), params, nullptr);
	}		

	void* ScriptMethod::getThunk()
	{
		return mThunk;
	}
}
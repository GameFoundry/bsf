#include "CmScriptClass.h"
#include "CmScriptMethod.h"
#include "CmUtil.h"
#include "CmException.h"

namespace CamelotFramework
{
	inline size_t ScriptClass::MethodId::Hash::operator()(const ScriptClass::MethodId& v) const
	{
		size_t seed = 0;
		hash_combine(seed, v.name);
		hash_combine(seed, v.numParams);
		return seed;
	}

	inline bool ScriptClass::MethodId::Equals::operator()(const ScriptClass::MethodId &a, const ScriptClass::MethodId &b) const
	{
		return a.name == b.name && a.numParams == b.numParams;
	}

	ScriptClass::MethodId::MethodId(const String& name, UINT32 numParams)
		:name(name), numParams(numParams)
	{

	}

	ScriptClass::ScriptClass(const String& fullName, MonoClass* monoClass)
		:mFullName(fullName), mClass(monoClass)
	{

	}

	ScriptClass::~ScriptClass()
	{
		for(auto& mapEntry : mMethods)
		{
			cm_delete(mapEntry.second);
		}

		mMethods.clear();
	}

	ScriptMethod& ScriptClass::getMethod(const String& name, UINT32 numParams)
	{
		MethodId mehodId(name, numParams);
		auto iterFind = mMethods.find(mehodId);
		if(iterFind != mMethods.end())
			return *iterFind->second;

		MonoMethod* method = mono_class_get_method_from_name(mClass, name.c_str(), (int)numParams);
		if(method == nullptr)
		{
			String fullMethodName = mFullName + "::" + name;
			CM_EXCEPT(InvalidParametersException, "Cannot get Mono method: " + fullMethodName);
		}

		ScriptMethod* newMethod = new (cm_alloc<ScriptMethod>()) ScriptMethod(method);
		mMethods[mehodId] = newMethod;

		return *newMethod;
	}

	MonoObject* ScriptClass::invokeMethod(const String& name, ScriptObject* instance, void** params, UINT32 numParams)
	{
		return getMethod(name, numParams).invoke(instance, params);
	}

	void ScriptClass::addInternalCall(const String& name, const void* method)
	{
		String fullMethodName = mFullName + "::" + name;
		mono_add_internal_call(fullMethodName.c_str(), method);
	}
}
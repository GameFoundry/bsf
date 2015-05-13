#include "BsScriptBoneWeight.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"

namespace BansheeEngine
{
	ScriptBoneWeight::ScriptBoneWeight(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptBoneWeight::initRuntimeData()
	{ }

	MonoObject* ScriptBoneWeight::box(const BoneWeight& value)
	{
		// We're casting away const but it's fine since structs are passed by value anyway
		return mono_value_box(MonoManager::instance().getDomain(),
			metaData.scriptClass->_getInternalClass(), (void*)&value);
	}

	BoneWeight ScriptBoneWeight::unbox(MonoObject* obj)
	{
		return *(BoneWeight*)mono_object_unbox(obj);
	}
}
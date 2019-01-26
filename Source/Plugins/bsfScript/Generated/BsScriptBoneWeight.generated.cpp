#include "BsScriptBoneWeight.generated.h"
#include "BsMonoMethod.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"

namespace bs
{
	ScriptBoneWeight::ScriptBoneWeight(MonoObject* managedInstance)
		:ScriptObject(managedInstance)
	{ }

	void ScriptBoneWeight::initRuntimeData()
	{ }

	MonoObject*ScriptBoneWeight::box(const BoneWeight& value)
	{
		return MonoUtil::box(metaData.scriptClass->_getInternalClass(), (void*)&value);
	}

	BoneWeight ScriptBoneWeight::unbox(MonoObject* value)
	{
		return *(BoneWeight*)MonoUtil::unbox(value);
	}

}

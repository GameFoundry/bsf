#include "BsScriptKeyframeRef.generated.h"
#include "BsMonoMethod.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"

namespace bs
{
	ScriptKeyframeRef::ScriptKeyframeRef(MonoObject* managedInstance)
		:ScriptObject(managedInstance)
	{ }

	void ScriptKeyframeRef::initRuntimeData()
	{ }

	MonoObject*ScriptKeyframeRef::box(const KeyframeRef& value)
	{
		return MonoUtil::box(metaData.scriptClass->_getInternalClass(), (void*)&value);
	}

	KeyframeRef ScriptKeyframeRef::unbox(MonoObject* value)
	{
		return *(KeyframeRef*)MonoUtil::unbox(value);
	}

}

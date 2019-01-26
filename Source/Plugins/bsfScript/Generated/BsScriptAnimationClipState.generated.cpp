#include "BsScriptAnimationClipState.generated.h"
#include "BsMonoMethod.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"

namespace bs
{
	ScriptAnimationClipState::ScriptAnimationClipState(MonoObject* managedInstance)
		:ScriptObject(managedInstance)
	{ }

	void ScriptAnimationClipState::initRuntimeData()
	{ }

	MonoObject*ScriptAnimationClipState::box(const AnimationClipState& value)
	{
		return MonoUtil::box(metaData.scriptClass->_getInternalClass(), (void*)&value);
	}

	AnimationClipState ScriptAnimationClipState::unbox(MonoObject* value)
	{
		return *(AnimationClipState*)MonoUtil::unbox(value);
	}

}

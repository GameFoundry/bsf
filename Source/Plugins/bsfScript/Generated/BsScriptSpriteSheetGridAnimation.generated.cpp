#include "BsScriptSpriteSheetGridAnimation.generated.h"
#include "BsMonoMethod.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"

namespace bs
{
	ScriptSpriteSheetGridAnimation::ScriptSpriteSheetGridAnimation(MonoObject* managedInstance)
		:ScriptObject(managedInstance)
	{ }

	void ScriptSpriteSheetGridAnimation::initRuntimeData()
	{ }

	MonoObject*ScriptSpriteSheetGridAnimation::box(const SpriteSheetGridAnimation& value)
	{
		return MonoUtil::box(metaData.scriptClass->_getInternalClass(), (void*)&value);
	}

	SpriteSheetGridAnimation ScriptSpriteSheetGridAnimation::unbox(MonoObject* value)
	{
		return *(SpriteSheetGridAnimation*)MonoUtil::unbox(value);
	}

}

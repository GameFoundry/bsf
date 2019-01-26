#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../../../Foundation/bsfCore/Animation/BsAnimationUtility.h"

namespace bs
{
	class BS_SCR_BE_EXPORT ScriptKeyframeRef : public ScriptObject<ScriptKeyframeRef>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "bs", "KeyframeRef")

		static MonoObject* box(const KeyframeRef& value);
		static KeyframeRef unbox(MonoObject* value);

	private:
		ScriptKeyframeRef(MonoObject* managedInstance);

	};
}

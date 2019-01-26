#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace bs
{
	class MorphShape;

	class BS_SCR_BE_EXPORT ScriptMorphShape : public ScriptObject<ScriptMorphShape>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "bs", "MorphShape")

		ScriptMorphShape(MonoObject* managedInstance, const SPtr<MorphShape>& value);

		SPtr<MorphShape> getInternal() const { return mInternal; }
		static MonoObject* create(const SPtr<MorphShape>& value);

	private:
		SPtr<MorphShape> mInternal;

		static MonoString* Internal_getName(ScriptMorphShape* thisPtr);
		static float Internal_getWeight(ScriptMorphShape* thisPtr);
	};
}

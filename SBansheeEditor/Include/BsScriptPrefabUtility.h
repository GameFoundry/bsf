#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptPrefabUtility : public ScriptObject <ScriptPrefabUtility>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "PrefabUtility")

	private:
		static void internal_breakPrefab(ScriptSceneObject* nativeInstance);
		static void internal_applyPrefab(ScriptSceneObject* nativeInstance);
		static void internal_revertPrefab(ScriptSceneObject* nativeInstance);

		ScriptPrefabUtility(MonoObject* instance);
	};
}
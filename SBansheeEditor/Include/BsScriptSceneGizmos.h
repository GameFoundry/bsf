#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	/** Interop class between C++ & CLR for SceneGizmos. */
	class BS_SCR_BED_EXPORT ScriptSceneGizmos : public ScriptObject <ScriptSceneGizmos>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "SceneGizmos")

	private:
		ScriptSceneGizmos(MonoObject* object, const SPtr<Camera>& camera);
		~ScriptSceneGizmos();

		SPtr<Camera> mCamera;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_Create(MonoObject* managedInstance, ScriptCamera* camera);
		static void internal_Draw(ScriptSceneGizmos* thisPtr);
	};
}
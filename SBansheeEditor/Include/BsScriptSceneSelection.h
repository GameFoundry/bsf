#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	/** Interop class between C++ & CLR for SceneSelection. */
	class BS_SCR_BED_EXPORT ScriptSceneSelection : public ScriptObject <ScriptSceneSelection>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "SceneSelection")

	private:
		ScriptSceneSelection(MonoObject* object, const SPtr<Camera>& camera);
		~ScriptSceneSelection();

		SPtr<Camera> mCamera;
		SelectionRenderer* mSelectionRenderer;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_Create(MonoObject* managedInstance, ScriptCamera* camera);
		static void internal_Draw(ScriptSceneSelection* thisPtr);
		static void internal_PickObject(ScriptSceneSelection* thisPtr, Vector2I* inputPos, bool additive);
	};
}
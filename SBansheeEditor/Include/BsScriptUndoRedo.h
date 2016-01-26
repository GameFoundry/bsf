//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for UndoRedo.
	 */
	class BS_SCR_BED_EXPORT ScriptUndoRedo : public ScriptObject <ScriptUndoRedo>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "UndoRedo");

	private:
		ScriptUndoRedo(MonoObject* instance);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_Undo();
		static void internal_Redo();
		static void internal_PushGroup(MonoString* name);
		static void internal_PopGroup(MonoString* name);
		static UINT32 internal_GetTopCommandId();
		static void internal_PopCommand(UINT32 id);
		static void internal_RecordSO(ScriptSceneObject* soPtr, bool recordHierarchy, MonoString* description);
		static MonoObject* internal_CloneSO(ScriptSceneObject* soPtr, MonoString* description);
		static MonoArray* internal_CloneSOMulti(MonoArray* soPtrs, MonoString* description);
		static MonoObject* internal_Instantiate(ScriptPrefab* prefabPtr, MonoString* description);
		static MonoObject* internal_CreateSO(MonoString* name, MonoString* description);
		static void internal_DeleteSO(ScriptSceneObject* soPtr, MonoString* description);
		static void internal_ReparentSO(ScriptSceneObject* soPtr, ScriptSceneObject* parentSOPtr, MonoString* description);
		static void internal_ReparentSOMulti(MonoArray* soPtrs, ScriptSceneObject* parentSOPtr, MonoString* description);
		static void internal_BreakPrefab(ScriptSceneObject* soPtr, MonoString* description);
	};
}
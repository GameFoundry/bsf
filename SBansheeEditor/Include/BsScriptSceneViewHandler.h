#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for SceneViewHandler.
	 */
	class BS_SCR_BED_EXPORT ScriptSceneViewHandler : public ScriptObject < ScriptSceneViewHandler >
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "SceneViewHandler")

	private:
		ScriptSceneViewHandler(MonoObject* object, EditorWidgetBase* parentWidget, const SPtr<Camera>& camera);
		~ScriptSceneViewHandler();

		SceneViewHandler* mHandler;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_Create(MonoObject* managedInstance, ScriptEditorWindow* parentWindow, ScriptCamera* camera);
		static void internal_Update(ScriptSceneViewHandler* thisPtr);
		static void internal_UpdateHandle(ScriptSceneViewHandler* thisPtr, Vector2I inputPos, Vector2I inputDelta);
		static void internal_UpdateSelection(ScriptSceneViewHandler* thisPtr);
		static void internal_TrySelectHandle(ScriptSceneViewHandler* thisPtr, Vector2I inputPos);
		static bool internal_IsHandleActive(ScriptSceneViewHandler* thisPtr, Vector2I inputPos);
		static void internal_ClearHandleSelection(ScriptSceneViewHandler* thisPtr, Vector2I inputPos);
		static void internal_PickObject(ScriptSceneViewHandler* thisPtr, Vector2I inputPos, bool additive);
	};
}
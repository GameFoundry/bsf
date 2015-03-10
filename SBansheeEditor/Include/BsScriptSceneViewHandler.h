#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptSceneViewHandler : public ScriptObject < ScriptSceneViewHandler >
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "SceneViewHandler")

	private:
		static void internal_Create(MonoObject* managedInstance, ScriptEditorWindow* parentWindow, ScriptCameraHandler* camera);
		static void internal_Update(ScriptSceneViewHandler* thisPtr);
		static void internal_UpdateHandle(ScriptSceneViewHandler* thisPtr, Vector2I inputPos, Vector2I inputDelta);
		static void internal_TrySelectHandle(ScriptSceneViewHandler* thisPtr, Vector2I inputPos);
		static bool internal_IsHandleActive(ScriptSceneViewHandler* thisPtr, Vector2I inputPos);
		static void internal_ClearHandleSelection(ScriptSceneViewHandler* thisPtr, Vector2I inputPos);
		static void internal_PickObject(ScriptSceneViewHandler* thisPtr, Vector2I inputPos, bool additive);

		ScriptSceneViewHandler(MonoObject* object, EditorWidgetBase* parentWidget, const SPtr<CameraHandler>& camera);
		~ScriptSceneViewHandler();

		SceneViewHandler* mHandler;
	};
}
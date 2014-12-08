#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptSceneViewHandler : public ScriptObject < ScriptSceneViewHandler >
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "SceneViewHandler")

	private:
		static void internal_Create(MonoObject* managedInstance, ScriptEditorWindow* parentWindow, ScriptCameraHandler* camera);
		static void internal_Update(ScriptSceneViewHandler* thisPtr, Vector2I inputPos, Vector2I inputDelta);
		static void internal_PointerPressed(ScriptSceneViewHandler* thisPtr, Vector2I inputPos);
		static void internal_PointerReleased(ScriptSceneViewHandler* thisPtr, Vector2I inputPos, bool controlPressed);

		ScriptSceneViewHandler(MonoObject* object, const EditorWidgetBase* parentWidget, const SPtr<CameraHandler>& camera);
		~ScriptSceneViewHandler();

		SceneViewHandler* mHandler;
	};
}
#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEditor
{
	class BS_SCR_BED_EXPORT ScriptModalWindow : public BansheeEngine::ScriptObject<ScriptModalWindow>
	{
	public:
		static void initMetaData();

	private:
		ScriptModalWindow(CM::INT32 left, CM::INT32 top, CM::UINT32 width, CM::UINT32 height);

		static void internal_createInstance(MonoObject* instance, CM::INT32 left, CM::INT32 top, CM::UINT32 width, CM::UINT32 height);
		static void internal_destroyInstance(ScriptModalWindow* nativeInstance);

		static void initRuntimeData();

		CM::RenderWindowPtr mRenderWindow;
		CM::HSceneObject mSceneObject;
		BS::HGUIWidget mGUI;
		BS::HCamera mCamera;
	};
}
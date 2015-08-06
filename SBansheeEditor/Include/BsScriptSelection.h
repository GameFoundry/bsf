#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptSelection : public ScriptObject<ScriptSelection>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "Selection");

		static void startUp();
		static void shutDown();

	private:
		ScriptSelection(MonoObject* instance);

		static void onSelectionChanged(const Vector<HSceneObject>& sceneObjects, const Vector<Path>& resPaths);
		static void onResourcePing(const Path& resPath);
		static void onSceneObjectPing(const HSceneObject& sceneObject);

		static void internal_GetSceneObjectSelection(MonoArray** selection);
		static void internal_SetSceneObjectSelection(MonoArray* selection);

		static void internal_GetResourceUUIDSelection(MonoArray** selection);
		static void internal_SetResourceUUIDSelection(MonoArray* selection);

		static void internal_GetResourcePathSelection(MonoArray** selection);
		static void internal_SetResourcePathSelection(MonoArray* selection);

		static void internal_PingResource(MonoString* resourcePath);
		static void internal_PingSceneObject(MonoObject* so);

		typedef void(__stdcall *OnSelectionChangedThunkDef) (MonoArray*, MonoArray*, MonoException**);
		typedef void(__stdcall *OnPingResourceThunkDef) (MonoString*, MonoException**);
		typedef void(__stdcall *OnPingSceneObjectThunkDef) (MonoObject*, MonoException**);

		static OnSelectionChangedThunkDef OnSelectionChangedThunk;
		static OnPingResourceThunkDef OnPingResourceThunk;
		static OnPingSceneObjectThunkDef OnPingSceneObjectThunk;

		static HEvent OnSelectionChangedConn;
		static HEvent OnPingResourceConn;
		static HEvent OnPingSceneObjectConn;
	};
}
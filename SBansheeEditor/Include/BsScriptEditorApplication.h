#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for EditorApplication.
	 */
	class BS_SCR_BED_EXPORT ScriptEditorApplication : public ScriptObject <ScriptEditorApplication>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "EditorApplication")

		/**
		 * @brief	Called every frame. Triggers delayed project load.
		 */
		 static void update();

	private:
		ScriptEditorApplication(MonoObject* instance);

		static bool mRequestProjectLoad;
		static bool mRequestAssemblyReload;
		static Path mProjectLoadPath;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_SetStatusScene(MonoString* name, bool modified);
		static void internal_SetStatusProject(bool modified);
		static void internal_SetStatusCompiling(bool compiling);
		static MonoString* internal_GetProjectPath();
		static MonoString* internal_GetProjectName();
		static bool internal_GetProjectLoaded();
		static MonoString* internal_GetCompilerPath();
		static MonoString* internal_GetBuiltinAssemblyPath();
		static MonoString* internal_GetScriptAssemblyPath();
		static MonoString* internal_GetFrameworkAssemblyPath();
		static MonoString* internal_GetEngineAssemblyName();
		static MonoString* internal_GetEditorAssemblyName();
		static MonoString* internal_GetScriptGameAssemblyName();
		static MonoString* internal_GetScriptEditorAssemblyName();
		static MonoString* internal_SaveScene(MonoString* path);
		static bool internal_IsValidProject(MonoString* path);
		static void internal_SaveProject();
		static void internal_LoadProject(MonoString* path);
		static void internal_UnloadProject();
		static void internal_CreateProject(MonoString* path);
		static void internal_ReloadAssemblies();
		static void internal_OpenExternally(MonoString* path);
		static void internal_RunUnitTests();

		typedef void(__stdcall *OnProjectLoadedThunkDef)(MonoException**);

		static OnProjectLoadedThunkDef onProjectLoadedThunk;
	};
}
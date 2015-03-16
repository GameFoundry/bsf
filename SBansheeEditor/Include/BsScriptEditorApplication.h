#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptEditorApplication : public ScriptObject <ScriptEditorApplication>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "EditorApplication")

	private:
		ScriptEditorApplication(MonoObject* instance);

		static MonoString* internal_GetProjectPath();
		static MonoString* internal_GetProjectName();
		static MonoString* internal_GetCompilerPath();
		static MonoString* internal_GetBuiltinAssemblyPath();
		static MonoString* internal_GetScriptAssemblyPath();
		static MonoString* internal_GetFrameworkAssemblyPath();
		static MonoString* internal_GetEngineAssemblyName();
		static MonoString* internal_GetEditorAssemblyName();
		static MonoString* internal_GetScriptGameAssemblyName();
		static MonoString* internal_GetScriptEditorAssemblyName();
	};
}
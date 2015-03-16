#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"
#include "BsBuildManager.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptBuildManager : public ScriptObject <ScriptBuildManager>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "BuildManager")

	private:
		ScriptBuildManager(MonoObject* instance);

		static MonoArray* internal_GetAvailablePlatforms();
		static PlatformType internal_GetActivePlatform();
		static void internal_SetActivePlatform(PlatformType value);
		static MonoObject* internal_GetActivePlatformInfo();
		static MonoObject* internal_GetPlatformInfo(PlatformType type);
		static MonoArray* internal_GetFrameworkAssemblies(PlatformType type);
		static MonoString* internal_GetMainExecutable(PlatformType type);
		static MonoString* internal_GetDefines(PlatformType type);
	};
}
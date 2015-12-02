#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"
#include "BsBuildManager.h"

namespace BansheeEngine
{
	/**
	 * @brief	Types of various folders used by the managed build manager.
	 */
	enum class ScriptBuildFolder
	{
		SourceRoot, /**< Absolute path to the root folder where all the prebuilt binaries and data exist. */
		DestinationRoot, /**< Absolute path to the root folder for a build for a specific platform. */
		NativeBinaries, /**< Folder where native binaries are stored. Relative to root. */
		BansheeAssemblies, /**< Folder where Banshee specific assemblies are stored. Relative to root. */
		FrameworkAssemblies, /**< Folder where .NET framework assemblies are stored. Relative to root. */
		Mono, /**< Folder where miscelaneous Mono files are stored. Relative to root. */
		Data /**< Folder where builtin data is stored. Relative to root. */
	};

	/**
	 * @brief	Interop class between C++ & CLR for BuildManager.
	 */
	class BS_SCR_BED_EXPORT ScriptBuildManager : public ScriptObject <ScriptBuildManager>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "BuildManager")

	private:
		ScriptBuildManager(MonoObject* instance);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static MonoArray* internal_GetAvailablePlatforms();
		static PlatformType internal_GetActivePlatform();
		static void internal_SetActivePlatform(PlatformType value);
		static MonoObject* internal_GetActivePlatformInfo();
		static MonoObject* internal_GetPlatformInfo(PlatformType type);
		static MonoArray* internal_GetFrameworkAssemblies(PlatformType type);
		static MonoString* internal_GetMainExecutable(PlatformType type);
		static MonoString* internal_GetDefines(PlatformType type);
		static MonoArray* internal_GetNativeBinaries(PlatformType type);
		static MonoString* internal_GetBuildFolder(ScriptBuildFolder folder, PlatformType platform);
		static void internal_InjectIcons(MonoString* filePath, ScriptPlatformInfo* info);
		static void internal_PackageResources(MonoString* buildFolder, ScriptPlatformInfo* info);
		static void internal_CreateStartupSettings(MonoString* buildFolder, ScriptPlatformInfo* info);
	};
}
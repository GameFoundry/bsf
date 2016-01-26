//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

namespace BansheeEngine
{
	static const char* ENGINE_ASSEMBLY = "MBansheeEngine";
	static const char* SCRIPT_GAME_ASSEMBLY = "MScriptGame";
	static const char* GAME_RESOURCES_FOLDER_NAME = "Resources\\";
	static const char* GAME_SETTINGS_NAME = "GameSettings.asset";
	static const char* GAME_RESOURCE_MANIFEST_NAME = "ResourceManifest.asset";

	/** Contains common engine paths. */
	class BS_EXPORT Paths
	{
	public:
		/**	Returns a path where the managed release assemblies are located. Relative to working directory. */
		static const Path& getReleaseAssemblyPath();

		/**	Returns a path where the managed debug assemblies are located. Relative to working directory. */
		static const Path& getDebugAssemblyPath();

		/**	Returns a path where the builtin assets are located. Relative to working directory. */
		static const Path& getRuntimeDataPath();

		/**	Returns a path where the builtin engine-specific assets are located. Relative to working directory. */
		static const Path& getEngineDataPath();

		/**	Returns a path to the game settings file used by editor-built executables. Relative to working directory. */
		static const Path& getGameSettingsPath();

		/**	Returns a path to the game resources folder used by editor-built executables. Relative to working directory. */
		static const Path& getGameResourcesPath();

		/**
		 * Searches common locations for a specified path by querying if the file/directory exists and returns the found 
		 * path.
		 * 			
		 * @param[in]	path	Relative path to search for (e.g. "Data\").				
		 * @return				Path at which the relative path was found at. This path will be relative to the working 
		 *						directory.
		 */
		static Path findPath(const Path& path);

		static const Path RELEASE_ASSEMBLY_PATH;
		static const Path DEBUG_ASSEMBLY_PATH;
		static const Path RUNTIME_DATA_PATH;
		static const Path ENGINE_DATA_PATH;
	};
}
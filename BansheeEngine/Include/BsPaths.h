#pragma once

namespace BansheeEngine
{
	static const char* ENGINE_ASSEMBLY = "MBansheeEngine";
	static const char* SCRIPT_GAME_ASSEMBLY = "MScriptGame";
	static const char* GAME_RESOURCES_FOLDER_NAME = "Resources\\";
	static const char* GAME_SETTINGS_NAME = "GameSettings.asset";
	static const char* GAME_RESOURCE_MANIFEST_NAME = "ResourceManifest.asset";

	/**
	 * @brief	Contains common engine paths.
	 */
	class BS_EXPORT Paths
	{
	public:
		/**
		 * @brief	Returns a path where the managed assemblies are located. Relative to working directory.
		 */
		static const Path& getAssemblyPath();

		/**
		 * @brief	Returns a path where the builtin assets are located. Relative to working directory.
		 */
		static const Path& getRuntimeDataPath();

		/**
		 * @brief	Returns a path where the builtin engine-specific assets are located. Relative to working directory.
		 */
		static const Path& getEngineDataPath();

		/**
		 * @brief	Returns a path to the game settings file used by editor-built executables. Relative to working directory.
		 */
		static const Path& getGameSettingsPath();

		/**
		 * @brief	Returns a path to the game resources folder used by editor-built executables. Relative to working directory.
		 */
		static const Path& getGameResourcesPath();

		/**
		 * @brief	Searches common locations for a specified path by querying if the file/directory exists and 
		 * 			returns the found path.
		 * 			
		 * @param	path	Relative path to search for (e.g. "Data\").
		 * 					
		 * @returns	Path at which the relative path was found at. This path will be relative to the working directory.
		 */
		static Path findPath(const Path& path);

		static const Path ASSEMBLY_PATH;
		static const Path RUNTIME_DATA_PATH;
		static const Path ENGINE_DATA_PATH;
	};
}
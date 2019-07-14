//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

namespace bs
{
	/** @addtogroup Utility-Engine-Internal
	 *  @{
	 */

#if BS_IS_BANSHEE3D || defined BS_IS_ASSET_TOOL
	#define BS_INCLUDE_B3D_PATHS 1
#else
	#define BS_INCLUDE_B3D_PATHS 0
#endif

#if BS_INCLUDE_B3D_PATHS
	constexpr const char* GAME_RESOURCES_FOLDER_NAME = "Resources/";
	constexpr const char* GAME_SETTINGS_NAME = "GameSettings.asset";
	constexpr const char* GAME_RESOURCE_MANIFEST_NAME = "ResourceManifest.asset";
	constexpr const char* GAME_RESOURCE_MAPPING_NAME = "ResourceMapping.asset";
#endif

	/** Contains common engine paths and utility method for searching for paths. */
	class BS_EXPORT Paths
	{
	public:	
		/**	Returns the absolute path where the builtin framework-specific assets are located. */
		static const Path& getDataPath();

		/** Returns the absolute path where the engine binaries are located in. */
		static const Path& getBinariesPath();

#if BS_INCLUDE_B3D_PATHS
		/**	Returns the absolute path where the builtin editor-specific assets are located. */
		static const Path& getEditorDataPath();

		/**	Returns the absolute path to the game settings file used by editor-built executables. */
		static const Path& getGameSettingsPath();

		/**	Returns the absolute path to the game resources folder used by editor-built executables. */
		static const Path& getGameResourcesPath();
#endif
		
		/**
		 * Searches common locations for a specified path by querying if the file/directory exists and returns the found
		 * path.
		 * 			
		 * @param[in]	path	Relative path to search for (for example "Data\").
		 * @return				Path at which the relative path was found at. This path will be relative to the working
		 *						directory.
		 */
		static Path findPath(const Path& path);

		/** Path to the root data directory. Relative to working directory, or RAW_APP_ROOT. */
		static const Path FRAMEWORK_DATA_PATH;
		
		/** Path where the release configuration managed assemblies are located at, relative to the working directory. */
		static const Path RELEASE_ASSEMBLY_PATH;

		/** Path where the debug configuration managed assemblies are located at, relative to the working directory. */
		static const Path DEBUG_ASSEMBLY_PATH;;

#if BS_INCLUDE_B3D_PATHS
		/** Path to the root editor data directory. Relative to working directory, or RAW_APP_ROOT. */
		static const Path EDITOR_DATA_PATH;
#endif
	};

	/** @} */
}

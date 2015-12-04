#include "BsPrerequisites.h"
#include "BsFileSystem.h"

namespace BansheeEngine
{
	static const Path RAW_APP_ROOT = "..\\..\\..\\"; // Path to the application root when files haven't been packaged yet (e.g. running from debugger)

	const Path Paths::RELEASE_ASSEMBLY_PATH = "bin\\Assemblies\\Release\\";
	const Path Paths::DEBUG_ASSEMBLY_PATH = "bin\\Assemblies\\Debug\\";
	const Path Paths::RUNTIME_DATA_PATH = "Data\\";
	const Path Paths::ENGINE_DATA_PATH = RUNTIME_DATA_PATH + "Engine\\";

	const Path& Paths::getReleaseAssemblyPath()
	{
		static Path path = findPath(RELEASE_ASSEMBLY_PATH);
		return path;
	}

	const Path& Paths::getDebugAssemblyPath()
	{
		static Path path = findPath(DEBUG_ASSEMBLY_PATH);
		return path;
	}

	const Path& Paths::getRuntimeDataPath()
	{
		static Path path = findPath(RUNTIME_DATA_PATH);
		return path;
	}

	const Path& Paths::getEngineDataPath()
	{
		static Path path = findPath(ENGINE_DATA_PATH);
		return path;
	}

	const Path& Paths::getGameSettingsPath()
	{
		static Path path = findPath(GAME_SETTINGS_NAME);
		return path;
	}

	const Path& Paths::getGameResourcesPath()
	{
		static Path path = findPath(GAME_RESOURCES_FOLDER_NAME);
		return path;
	}

	Path Paths::findPath(const Path& path)
	{
		if (FileSystem::exists(path))
			return path;

		Path output = path;
		output.makeAbsolute(RAW_APP_ROOT);
		if (FileSystem::exists(output))
			return output;

		// No path found, but return the initial value by default
		return path;
	}
}
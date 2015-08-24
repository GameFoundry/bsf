#include "BsEditorScriptLibrary.h"
#include "BsEditorScriptManager.h"
#include "BsEditorApplication.h"
#include "BsScriptObjectManager.h"
#include "BsFileSystem.h"

namespace BansheeEngine
{
	void EditorScriptLibrary::initialize()
	{
		EngineScriptLibrary::initialize();

		EditorScriptManager::startUp();
	}

	void EditorScriptLibrary::reload()
	{
		Vector<std::pair<String, Path>> assemblies;

		Path engineAssemblyPath = gApplication().getEngineAssemblyPath();
		assemblies.push_back({ ENGINE_ASSEMBLY, engineAssemblyPath });

		if (gEditorApplication().isProjectLoaded())
		{
			Path gameAssemblyPath = gApplication().getGameAssemblyPath();
			if (FileSystem::exists(gameAssemblyPath))
				assemblies.push_back({ SCRIPT_GAME_ASSEMBLY, gameAssemblyPath });
		}

		String editorAssemblyPath = gEditorApplication().getEditorAssemblyPath().toString();
		assemblies.push_back({ EDITOR_ASSEMBLY, editorAssemblyPath });

		if (gEditorApplication().isProjectLoaded())
		{
			Path editorScriptAssemblyPath = gEditorApplication().getEditorScriptAssemblyPath();
			if (FileSystem::exists(editorScriptAssemblyPath))
				assemblies.push_back({ SCRIPT_EDITOR_ASSEMBLY, editorScriptAssemblyPath });
		}

		ScriptObjectManager::instance().refreshAssemblies(assemblies);
	}

	void EditorScriptLibrary::destroy()
	{
		EditorScriptManager::shutDown();

		EngineScriptLibrary::destroy();
	}
}
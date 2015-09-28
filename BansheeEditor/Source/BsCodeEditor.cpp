#include "BsCodeEditor.h"
#include "BsEditorApplication.h"
#include "BsProjectLibrary.h"
#include "BsProjectResourceMeta.h"
#include "BsScriptCodeImportOptions.h"
#include "BsBuildManager.h"

#if BS_PLATFORM == BS_PLATFORM_WIN32
#include "Win32/BsVSCodeEditor.h"
#else
static_assert("Make sure to add implementations for other platforms.");
#endif

namespace BansheeEngine
{
	CodeEditorManager::CodeEditorManager()
		:mActiveEditor(nullptr)
	{
#if BS_PLATFORM == BS_PLATFORM_WIN32
		VSCodeEditorFactory* vsCodeEditorFactory = bs_new<VSCodeEditorFactory>();
		Vector<CodeEditorType> vsEditors = vsCodeEditorFactory->getAvailableEditors();
		for(auto& editor : vsEditors)
		{
			mFactoryPerEditor[editor] = vsCodeEditorFactory;
			mEditors.push_back(editor);
		}

		mFactories.push_back(vsCodeEditorFactory);
#else
		static_assert("Make sure to add implementations for other platforms.");
#endif
	}

	CodeEditorManager::~CodeEditorManager()
	{
		for (auto& factory : mFactories)
			bs_delete(factory);

		if (mActiveEditor != nullptr)
			bs_delete(mActiveEditor);
	}

	void CodeEditorManager::setActive(CodeEditorType editor)
	{
		if (mActiveEditor != nullptr)
		{
			bs_delete(mActiveEditor);
			mActiveEditor = nullptr;
		}

		auto findIter = mFactoryPerEditor.find(editor);
		if (findIter == mFactoryPerEditor.end())
			return;

		mActiveEditor = findIter->second->create(editor);
	}

	void CodeEditorManager::openFile(const Path& path, UINT32 lineNumber) const
	{
		if (mActiveEditor == nullptr)
			return;

		Path filePath = path;
		if (!path.isAbsolute())
			filePath.makeAbsolute(gEditorApplication().getProjectPath());

		mActiveEditor->openFile(getSolutionPath(), filePath, lineNumber);
	}

	void CodeEditorManager::syncSolution() const
	{
		if (mActiveEditor == nullptr)
			return;

		CodeSolutionData slnData;
		slnData.name = gEditorApplication().getProjectName();

		Vector<UINT32> scriptTypeIds =
		{
			TID_ScriptCode, TID_PlainText, TID_Shader, TID_ShaderInclude
		};

		Vector<ProjectLibrary::LibraryEntry*> libraryEntries = gProjectLibrary().search(L"*", scriptTypeIds);
		
		PlatformType activePlatform = BuildManager::instance().getActivePlatform();
		Vector<WString> frameworkAssemblies = BuildManager::instance().getFrameworkAssemblies(activePlatform);

		slnData.projects.push_back(CodeProjectData());
		slnData.projects.push_back(CodeProjectData());

		// Game project
		CodeProjectData& gameProject = slnData.projects[0];
		gameProject.name = toWString(SCRIPT_GAME_ASSEMBLY);
		gameProject.defines = BuildManager::instance().getDefines(activePlatform);
		
		//// Add references
		gameProject.assemblyReferences.push_back(CodeProjectReference{ toWString(ENGINE_ASSEMBLY), gApplication().getEngineAssemblyPath() });
		for (auto& assemblyName : frameworkAssemblies)
			gameProject.assemblyReferences.push_back(CodeProjectReference{ assemblyName, Path::BLANK });

		// Editor project
		CodeProjectData& editorProject = slnData.projects[1];
		editorProject.name = toWString(SCRIPT_EDITOR_ASSEMBLY);

		//// Add references
		editorProject.assemblyReferences.push_back(CodeProjectReference{ toWString(ENGINE_ASSEMBLY), gApplication().getEngineAssemblyPath() });
		editorProject.assemblyReferences.push_back(CodeProjectReference{ toWString(EDITOR_ASSEMBLY), gEditorApplication().getEditorAssemblyPath() });
		for (auto& assemblyName : frameworkAssemblies)
			gameProject.assemblyReferences.push_back(CodeProjectReference{ assemblyName, Path::BLANK });

		editorProject.projectReferences.push_back(CodeProjectReference{ gameProject.name, Path::BLANK });

		//// Add files for both projects
		for (auto& entry : libraryEntries)
		{
			if (entry->type != ProjectLibrary::LibraryEntryType::File)
				continue;

			ProjectLibrary::ResourceEntry* resEntry = static_cast<ProjectLibrary::ResourceEntry*>(entry);
			if (resEntry->meta->getTypeID() == TID_ScriptCode)
			{
				SPtr<ScriptCodeImportOptions> scriptIO = std::static_pointer_cast<ScriptCodeImportOptions>(resEntry->meta->getImportOptions());

				bool isEditorScript = false;
				if (scriptIO != nullptr)
					isEditorScript = scriptIO->isEditorScript();

				if (isEditorScript)
					editorProject.codeFiles.push_back(resEntry->path);
				else
					gameProject.codeFiles.push_back(resEntry->path);
			}
			else
				gameProject.nonCodeFiles.push_back(resEntry->path);
		}

		mActiveEditor->syncSolution(slnData, gEditorApplication().getProjectPath());
	}

	Path CodeEditorManager::getSolutionPath() const
	{
		Path path = gEditorApplication().getProjectPath();
		path.append(gEditorApplication().getProjectName() + L".sln");

		return path;
	}
}
#include "BsScriptEditorApplication.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoUtil.h"
#include "BsEditorApplication.h"
#include "BsProjectLibrary.h"
#include "BsProjectResourceMeta.h"
#include "BsPrefab.h"
#include "BsPrefabUtility.h"
#include "BsSceneManager.h"
#include "BsPlatform.h"
#include "BsResources.h"
#include "BsScriptEditorWindow.h"
#include "BsEditorWindowBase.h"
#include "BsProfilerOverlay.h"
#include "BsCGUIWidget.h"
#include "BsSceneObject.h"
#include "BsCCamera.h"

namespace BansheeEngine
{
	bool ScriptEditorApplication::mRequestProjectLoad = false;
	Path ScriptEditorApplication::mProjectLoadPath;

	ScriptEditorApplication::OnProjectLoadedThunkDef ScriptEditorApplication::onProjectLoadedThunk;

	ScriptEditorApplication::ScriptEditorApplication(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptEditorApplication::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_GetProjectPath", &ScriptEditorApplication::internal_GetProjectPath);
		metaData.scriptClass->addInternalCall("Internal_GetProjectName", &ScriptEditorApplication::internal_GetProjectName);
		metaData.scriptClass->addInternalCall("Internal_GetProjectLoaded", &ScriptEditorApplication::internal_GetProjectLoaded);
		metaData.scriptClass->addInternalCall("Internal_GetCompilerPath", &ScriptEditorApplication::internal_GetCompilerPath);
		metaData.scriptClass->addInternalCall("Internal_GetBuiltinAssemblyPath", &ScriptEditorApplication::internal_GetBuiltinAssemblyPath);
		metaData.scriptClass->addInternalCall("Internal_GetScriptAssemblyPath", &ScriptEditorApplication::internal_GetScriptAssemblyPath);
		metaData.scriptClass->addInternalCall("Internal_GetFrameworkAssemblyPath", &ScriptEditorApplication::internal_GetFrameworkAssemblyPath);
		metaData.scriptClass->addInternalCall("Internal_GetEngineAssemblyName", &ScriptEditorApplication::internal_GetEngineAssemblyName);
		metaData.scriptClass->addInternalCall("Internal_GetEditorAssemblyName", &ScriptEditorApplication::internal_GetEditorAssemblyName);
		metaData.scriptClass->addInternalCall("Internal_GetScriptGameAssemblyName", &ScriptEditorApplication::internal_GetScriptGameAssemblyName);
		metaData.scriptClass->addInternalCall("Internal_GetScriptEditorAssemblyName", &ScriptEditorApplication::internal_GetScriptEditorAssemblyName);
		metaData.scriptClass->addInternalCall("Internal_SaveScene", &ScriptEditorApplication::internal_SaveScene);
		metaData.scriptClass->addInternalCall("Internal_IsValidProject", &ScriptEditorApplication::internal_IsValidProject);
		metaData.scriptClass->addInternalCall("Internal_SaveProject", &ScriptEditorApplication::internal_SaveProject);
		metaData.scriptClass->addInternalCall("Internal_LoadProject", &ScriptEditorApplication::internal_LoadProject);
		metaData.scriptClass->addInternalCall("Internal_UnloadProject", &ScriptEditorApplication::internal_UnloadProject);
		metaData.scriptClass->addInternalCall("Internal_CreateProject", &ScriptEditorApplication::internal_CreateProject);
		metaData.scriptClass->addInternalCall("Internal_OpenExternally", &ScriptEditorApplication::internal_OpenExternally);

		onProjectLoadedThunk = (OnProjectLoadedThunkDef)metaData.scriptClass->getMethod("OnProjectLoaded")->getThunk();
	}

	void ScriptEditorApplication::update()
	{
		// Project load must be delayed when requested from managed code because it
		// triggers managed assembly reload, and that can't be performed when called
		// from the Mono thread.
		if (mRequestProjectLoad)
		{
			gEditorApplication().loadProject(mProjectLoadPath);

			mRequestProjectLoad = false;
			MonoUtil::invokeThunk(onProjectLoadedThunk);
		}
	}

	MonoString* ScriptEditorApplication::internal_GetProjectPath()
	{
		Path projectPath = gEditorApplication().getProjectPath();

		return MonoUtil::wstringToMono(MonoManager::instance().getDomain(), projectPath.toWString());
	}

	MonoString* ScriptEditorApplication::internal_GetProjectName()
	{
		WString projectName = gEditorApplication().getProjectName();

		return MonoUtil::wstringToMono(MonoManager::instance().getDomain(), projectName);
	}

	bool ScriptEditorApplication::internal_GetProjectLoaded()
	{
		return gEditorApplication().isProjectLoaded();
	}

	MonoString* ScriptEditorApplication::internal_GetCompilerPath()
	{
		Path compilerPath = MonoManager::instance().getCompilerPath();

		return MonoUtil::wstringToMono(MonoManager::instance().getDomain(), compilerPath.toWString());
	}

	MonoString* ScriptEditorApplication::internal_GetBuiltinAssemblyPath()
	{
		Path assemblyFolder = gEditorApplication().getBuiltinAssemblyFolder();

		return MonoUtil::wstringToMono(MonoManager::instance().getDomain(), assemblyFolder.toWString());
	}

	MonoString* ScriptEditorApplication::internal_GetScriptAssemblyPath()
	{
		Path assemblyFolder = gEditorApplication().getScriptAssemblyFolder();

		return MonoUtil::wstringToMono(MonoManager::instance().getDomain(), assemblyFolder.toWString());
	}

	MonoString* ScriptEditorApplication::internal_GetFrameworkAssemblyPath()
	{
		Path assemblyFolder = MonoManager::instance().getFrameworkAssembliesFolder();

		return MonoUtil::wstringToMono(MonoManager::instance().getDomain(), assemblyFolder.toWString());
	}

	MonoString* ScriptEditorApplication::internal_GetEngineAssemblyName()
	{
		return MonoUtil::wstringToMono(MonoManager::instance().getDomain(), toWString(ENGINE_ASSEMBLY) + L".dll");
	}

	MonoString* ScriptEditorApplication::internal_GetEditorAssemblyName()
	{
		return MonoUtil::wstringToMono(MonoManager::instance().getDomain(), toWString(EDITOR_ASSEMBLY) + L".dll");
	}

	MonoString* ScriptEditorApplication::internal_GetScriptGameAssemblyName()
	{
		return MonoUtil::wstringToMono(MonoManager::instance().getDomain(), toWString(SCRIPT_GAME_ASSEMBLY) + L".dll");
	}

	MonoString* ScriptEditorApplication::internal_GetScriptEditorAssemblyName()
	{
		return MonoUtil::wstringToMono(MonoManager::instance().getDomain(), toWString(SCRIPT_EDITOR_ASSEMBLY) + L".dll");
	}

	MonoString* ScriptEditorApplication::internal_SaveScene(MonoString* path)
	{
		Path nativePath = MonoUtil::monoToWString(path);
		HSceneObject sceneRoot = gSceneManager().getRootNode();
		
		ProjectLibrary::LibraryEntry* entry = gProjectLibrary().findEntry(nativePath);
		HPrefab scene;
		if (entry != nullptr)
		{
			if (entry->type == ProjectLibrary::LibraryEntryType::Directory)
				return nullptr;

			ProjectLibrary::ResourceEntry* resEntry = static_cast<ProjectLibrary::ResourceEntry*>(entry);
			if (resEntry->meta->getTypeID() != TID_Prefab)
				return nullptr;

			scene = static_resource_cast<Prefab>(gProjectLibrary().load(nativePath));
			scene->update(sceneRoot);

			PrefabUtility::recordPrefabDiff(sceneRoot);
			gProjectLibrary().saveEntry(scene);
		}
		else
		{
			scene = Prefab::create(sceneRoot);
			PrefabUtility::recordPrefabDiff(sceneRoot);
			gProjectLibrary().createEntry(scene, nativePath);
		}

		return MonoUtil::stringToMono(MonoManager::instance().getDomain(), scene.getUUID());
	}

	bool ScriptEditorApplication::internal_IsValidProject(MonoString* path)
	{
		Path nativePath = MonoUtil::monoToWString(path);
		return gEditorApplication().isValidProjectPath(nativePath);
	}

	void ScriptEditorApplication::internal_SaveProject()
	{
		gEditorApplication().saveProject();
	}

	void ScriptEditorApplication::internal_LoadProject(MonoString* path)
	{
		mRequestProjectLoad = true;
		mProjectLoadPath = MonoUtil::monoToWString(path);
	}

	void ScriptEditorApplication::internal_UnloadProject()
	{
		gEditorApplication().unloadProject();
	}

	void ScriptEditorApplication::internal_CreateProject(MonoString* path)
	{
		Path nativePath = MonoUtil::monoToWString(path);

		gEditorApplication().createProject(nativePath);
	}

	void ScriptEditorApplication::internal_OpenExternally(MonoString* path)
	{
		Path nativePath = MonoUtil::monoToWString(path);

		Platform::open(nativePath);
	}
}

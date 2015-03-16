#include "BsScriptEditorApplication.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoUtil.h"
#include "BsEditorApplication.h"

namespace BansheeEngine
{
	ScriptEditorApplication::ScriptEditorApplication(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptEditorApplication::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_GetProjectPath", &ScriptEditorApplication::internal_GetProjectPath);
		metaData.scriptClass->addInternalCall("Internal_GetProjectName", &ScriptEditorApplication::internal_GetProjectName);
		metaData.scriptClass->addInternalCall("Internal_GetCompilerPath", &ScriptEditorApplication::internal_GetCompilerPath);
		metaData.scriptClass->addInternalCall("Internal_GetBuiltinAssemblyPath", &ScriptEditorApplication::internal_GetBuiltinAssemblyPath);
		metaData.scriptClass->addInternalCall("Internal_GetScriptAssemblyPath", &ScriptEditorApplication::internal_GetScriptAssemblyPath);
		metaData.scriptClass->addInternalCall("Internal_GetFrameworkAssemblyPath", &ScriptEditorApplication::internal_GetFrameworkAssemblyPath);
		metaData.scriptClass->addInternalCall("Internal_GetEngineAssemblyName", &ScriptEditorApplication::internal_GetEngineAssemblyName);
		metaData.scriptClass->addInternalCall("Internal_GetEditorAssemblyName", &ScriptEditorApplication::internal_GetEditorAssemblyName);
		metaData.scriptClass->addInternalCall("Internal_GetScriptGameAssemblyName", &ScriptEditorApplication::internal_GetScriptGameAssemblyName);
		metaData.scriptClass->addInternalCall("Internal_GetScriptEditorAssemblyName", &ScriptEditorApplication::internal_GetScriptEditorAssemblyName);
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
}

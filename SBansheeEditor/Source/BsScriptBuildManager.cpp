#include "BsScriptBuildManager.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoUtil.h"
#include "BsScriptPlatformInfo.h"

namespace BansheeEngine
{
	ScriptBuildManager::ScriptBuildManager(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptBuildManager::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("internal_GetAvailablePlatforms", &ScriptBuildManager::internal_GetAvailablePlatforms);
		metaData.scriptClass->addInternalCall("internal_GetActivePlatform", &ScriptBuildManager::internal_GetActivePlatform);
		metaData.scriptClass->addInternalCall("internal_SetActivePlatform", &ScriptBuildManager::internal_SetActivePlatform);
		metaData.scriptClass->addInternalCall("internal_GetActivePlatformInfo", &ScriptBuildManager::internal_GetActivePlatformInfo);
		metaData.scriptClass->addInternalCall("internal_GetPlatformInfo", &ScriptBuildManager::internal_GetPlatformInfo);
		metaData.scriptClass->addInternalCall("internal_GetFrameworkAssemblies", &ScriptBuildManager::internal_GetFrameworkAssemblies);
		metaData.scriptClass->addInternalCall("internal_GetMainExecutable", &ScriptBuildManager::internal_GetMainExecutable);
		metaData.scriptClass->addInternalCall("internal_GetDefines", &ScriptBuildManager::internal_GetDefines);
	}

	MonoArray* ScriptBuildManager::internal_GetAvailablePlatforms()
	{
		const Vector<PlatformType>& availableType = BuildManager::instance().getAvailablePlatforms();

		ScriptArray outArray = ScriptArray::create<UINT32>((UINT32)availableType.size());
		UINT32 idx = 0;
		for (auto& type : availableType)
			outArray.set(idx++, type);

		return outArray.getInternal();
	}

	PlatformType ScriptBuildManager::internal_GetActivePlatform()
	{
		return BuildManager::instance().getActivePlatform();
	}

	void ScriptBuildManager::internal_SetActivePlatform(PlatformType value)
	{
		BuildManager::instance().setActivePlatform(value);
	}

	MonoObject* ScriptBuildManager::internal_GetActivePlatformInfo()
	{
		return ScriptPlatformInfo::create(BuildManager::instance().getActivePlatformInfo());
	}

	MonoObject* ScriptBuildManager::internal_GetPlatformInfo(PlatformType type)
	{
		return ScriptPlatformInfo::create(BuildManager::instance().getPlatformInfo(type));
	}

	MonoArray* ScriptBuildManager::internal_GetFrameworkAssemblies(PlatformType type)
	{
		Vector<WString> frameworkAssemblies = BuildManager::instance().getFrameworkAssemblies(type);

		ScriptArray outArray = ScriptArray::create<WString>((UINT32)frameworkAssemblies.size());
		UINT32 idx = 0;
		for (auto& assemblyName : frameworkAssemblies)
			outArray.set(idx++, assemblyName);

		return outArray.getInternal();
	}

	MonoString* ScriptBuildManager::internal_GetMainExecutable(PlatformType type)
	{
		return MonoUtil::wstringToMono(MonoManager::instance().getDomain(), BuildManager::instance().getMainExecutable(type).toWString());
	}

	MonoString* ScriptBuildManager::internal_GetDefines(PlatformType type)
	{
		return MonoUtil::wstringToMono(MonoManager::instance().getDomain(), BuildManager::instance().getDefines(type));
	}
}

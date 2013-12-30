#include "BsScriptStringTable.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	void ScriptStringTable::initMetaData()
	{
		metaData = ScriptMeta("MBansheeEngine", "BansheeEngine", "StringTable", &ScriptStringTable::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}
	void ScriptStringTable::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_GetActiveLanguage", &ScriptStringTable::internal_GetActiveLanguage);
		metaData.scriptClass->addInternalCall("Internal_SetActiveLanguage", &ScriptStringTable::internal_SetActiveLanguage);

		metaData.scriptClass->addInternalCall("Internal_SetString", &ScriptStringTable::internal_SetString);
		metaData.scriptClass->addInternalCall("Internal_RemoveString", &ScriptStringTable::internal_RemoveString);
		metaData.scriptClass->addInternalCall("Internal_GetLocalizedString", &ScriptStringTable::internal_GetLocalizedString);
	}

	void ScriptStringTable::internal_GetActiveLanguage(CM::Language* value)
	{
		*value = StringTable::instance().getActiveLanguage();
	}

	void ScriptStringTable::internal_SetActiveLanguage(CM::Language value)
	{
		StringTable::instance().setActiveLanguage(value);
	}

	void ScriptStringTable::internal_SetString(MonoString* identifier, CM::Language language, MonoString* value)
	{
		StringTable::instance().setString(MonoUtil::monoToWString(identifier), language, MonoUtil::monoToWString(value));
	}

	void ScriptStringTable::internal_RemoveString(MonoString* identifier)
	{
		StringTable::instance().removeString(MonoUtil::monoToWString(identifier));
	}

	void ScriptStringTable::internal_GetLocalizedString(MonoString* identifier, MonoString** value)
	{
		CM::HString stringHandle(MonoUtil::monoToWString(identifier));
		WString outString = stringHandle;

		*value = MonoUtil::wstringToMono(MonoManager::instance().getDomain(), outString);
	}
}
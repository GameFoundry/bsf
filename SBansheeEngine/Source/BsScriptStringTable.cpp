#include "BsScriptStringTable.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsScriptResourceManager.h"
#include "BsStringTableManager.h"

namespace BansheeEngine
{
	ScriptStringTable::ScriptStringTable(MonoObject* instance, const HStringTable& table)
		:TScriptResource(instance, table)
	{ }

	void ScriptStringTable::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptStringTable::internal_CreateInstance);

		metaData.scriptClass->addInternalCall("Internal_GetNumStrings", &ScriptStringTable::internal_GetNumStrings);
		metaData.scriptClass->addInternalCall("Internal_GetIdentifiers", &ScriptStringTable::internal_GetIdentifiers);

		metaData.scriptClass->addInternalCall("Internal_SetString", &ScriptStringTable::internal_SetString);
		metaData.scriptClass->addInternalCall("Internal_SetStringDefault", &ScriptStringTable::internal_SetStringDefault);
		metaData.scriptClass->addInternalCall("Internal_RemoveString", &ScriptStringTable::internal_RemoveString);
		metaData.scriptClass->addInternalCall("Internal_GetString", &ScriptStringTable::internal_GetString);
		metaData.scriptClass->addInternalCall("Internal_GetStringDefault", &ScriptStringTable::internal_GetStringDefault);
	}

	void ScriptStringTable::internal_CreateInstance(MonoObject* instance)
	{
		HStringTable stringTable = StringTable::create();

		ScriptStringTable* scriptInstance;
		ScriptResourceManager::instance().createScriptResource(instance, stringTable, &scriptInstance);
	}

	UINT32 ScriptStringTable::internal_GetNumStrings(ScriptStringTable* thisPtr)
	{
		return (UINT32)thisPtr->getHandle()->getIdentifiers().size();
	}

	MonoArray* ScriptStringTable::internal_GetIdentifiers(ScriptStringTable* thisPtr)
	{
		const UnorderedSet<WString>& identifiers = thisPtr->getHandle()->getIdentifiers();
		UINT32 numIdentifiers = (UINT32)identifiers.size();

		ScriptArray outArray = ScriptArray::create<WString>(numIdentifiers);
		UINT32 idx = 0;
		for (auto identifier : identifiers)
		{
			outArray.set(idx, identifier);

			idx++;
		}

		return outArray.getInternal();
	}

	void ScriptStringTable::internal_SetString(ScriptStringTable* thisPtr, MonoString* identifier, Language language, MonoString* value)
	{
		WString nativeIdentifier = MonoUtil::monoToWString(identifier);
		WString nativeValue = MonoUtil::monoToWString(value);

		thisPtr->getHandle()->setString(nativeIdentifier, language, nativeValue);
	}

	void ScriptStringTable::internal_SetStringDefault(ScriptStringTable* thisPtr, MonoString* identifier, MonoString* value)
	{
		WString nativeIdentifier = MonoUtil::monoToWString(identifier);
		WString nativeValue = MonoUtil::monoToWString(value);

		thisPtr->getHandle()->setString(nativeIdentifier, StringTableManager::instance().getActiveLanguage(), nativeValue);
	}

	void ScriptStringTable::internal_RemoveString(ScriptStringTable* thisPtr, MonoString* identifier)
	{
		WString nativeIdentifier = MonoUtil::monoToWString(identifier);
		thisPtr->getHandle()->removeString(nativeIdentifier);
	}

	void ScriptStringTable::internal_GetString(ScriptStringTable* thisPtr, MonoString* identifier, Language language, MonoString** value)
	{
		WString nativeIdentifier = MonoUtil::monoToWString(identifier);
		WString nativeValue = thisPtr->getHandle()->getString(nativeIdentifier, language);

		*value = MonoUtil::wstringToMono(MonoManager::instance().getDomain(), nativeValue);
	}

	void ScriptStringTable::internal_GetStringDefault(ScriptStringTable* thisPtr, MonoString* identifier, MonoString** value)
	{
		WString nativeIdentifier = MonoUtil::monoToWString(identifier);
		WString nativeValue = thisPtr->getHandle()->getString(nativeIdentifier, StringTableManager::instance().getActiveLanguage());

		*value = MonoUtil::wstringToMono(MonoManager::instance().getDomain(), nativeValue);
	}
}
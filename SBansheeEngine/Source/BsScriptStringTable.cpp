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
		:ScriptObject(instance), mTable(table)
	{ }

	void ScriptStringTable::initRuntimeData()
	{
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

		ScriptResourceManager::instance().createScriptStringTable(instance, stringTable);
	}

	UINT32 ScriptStringTable::internal_GetNumStrings(ScriptStringTable* thisPtr)
	{
		return (UINT32)thisPtr->getStringTableHandle()->getIdentifiers().size();
	}

	MonoArray* ScriptStringTable::internal_GetIdentifiers(ScriptStringTable* thisPtr)
	{
		const UnorderedSet<WString>& identifiers = thisPtr->getStringTableHandle()->getIdentifiers();
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

		thisPtr->getStringTableHandle()->setString(nativeIdentifier, language, nativeValue);
	}

	void ScriptStringTable::internal_SetStringDefault(ScriptStringTable* thisPtr, MonoString* identifier, MonoString* value)
	{
		WString nativeIdentifier = MonoUtil::monoToWString(identifier);
		WString nativeValue = MonoUtil::monoToWString(value);

		thisPtr->getStringTableHandle()->setString(nativeIdentifier, StringTableManager::instance().getActiveLanguage(), nativeValue);
	}

	void ScriptStringTable::internal_RemoveString(ScriptStringTable* thisPtr, MonoString* identifier)
	{
		WString nativeIdentifier = MonoUtil::monoToWString(identifier);
		thisPtr->getStringTableHandle()->removeString(nativeIdentifier);
	}

	void ScriptStringTable::internal_GetString(ScriptStringTable* thisPtr, MonoString* identifier, Language language, MonoString** value)
	{
		WString nativeIdentifier = MonoUtil::monoToWString(identifier);
		WString nativeValue = thisPtr->getStringTableHandle()->getString(nativeIdentifier, language);

		*value = MonoUtil::wstringToMono(MonoManager::instance().getDomain(), nativeValue);
	}

	void ScriptStringTable::internal_GetStringDefault(ScriptStringTable* thisPtr, MonoString* identifier, MonoString** value)
	{
		WString nativeIdentifier = MonoUtil::monoToWString(identifier);
		WString nativeValue = thisPtr->getStringTableHandle()->getString(nativeIdentifier, StringTableManager::instance().getActiveLanguage());

		*value = MonoUtil::wstringToMono(MonoManager::instance().getDomain(), nativeValue);
	}

	void ScriptStringTable::setNativeHandle(const HResource& resource)
	{
		mTable = static_resource_cast<StringTable>(resource);
	}

	void ScriptStringTable::_onManagedInstanceDeleted()
	{
		mManagedInstance = nullptr;

		if (!mRefreshInProgress)
			ScriptResourceManager::instance().destroyScriptResource(this);
	}
}
#include "BsScriptHString.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptHString::ScriptHString(const HString& string)
		:mString(string)
	{ }

	void ScriptHString::initMetaData()
	{
		metaData = ScriptMeta(BansheeEngineAssemblyName, "BansheeEngine", "LocString", &ScriptHString::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void ScriptHString::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptHString::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptHString::internal_destroyInstance);
		metaData.scriptClass->addInternalCall("Internal_SetParameter", &ScriptHString::internal_setParameter);
		metaData.scriptClass->addInternalCall("Internal_GetValue", &ScriptHString::internal_getValue);
	}

	void ScriptHString::internal_createInstance(MonoObject* instance, MonoString* identifier)
	{
		HString string(MonoUtil::monoToWString(identifier));
		
		ScriptHString* nativeInstance = new (cm_alloc<ScriptHString>()) ScriptHString(string);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, &nativeInstance);
	}

	void ScriptHString::internal_destroyInstance(ScriptHString* nativeInstance)
	{
		cm_delete(nativeInstance);
	}

	void ScriptHString::internal_setParameter(HString* nativeInstance, UINT32 idx, MonoString* value)
	{
		nativeInstance->setParameter(idx, MonoUtil::monoToWString(value));
	}

	void ScriptHString::internal_getValue(CM::HString* nativeInstance, MonoString** value)
	{
		*value = MonoUtil::wstringToMono(MonoManager::instance().getDomain(), nativeInstance->getValue());
	}
}
#include "BsScriptSerializableField.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsManagedSerializableObjectInfo.h"

namespace BansheeEngine
{
	ScriptSerializableField::ScriptSerializableField(const ManagedSerializableFieldInfoPtr& fieldInfo)
		:mFieldInfo(fieldInfo)
	{

	}

	void ScriptSerializableField::initMetaData()
	{
		metaData = ScriptMeta(BansheeEngineAssemblyName, "BansheeEngine", "SerializableField", &ScriptSerializableField::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void ScriptSerializableField::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptSerializableField::internal_destroyInstance);
		metaData.scriptClass->addInternalCall("Internal_GetValue", &ScriptSerializableField::internal_getValue);
		metaData.scriptClass->addInternalCall("Internal_SetValue", &ScriptSerializableField::internal_setValue);
	}

	ScriptSerializableField* ScriptSerializableField::create(MonoObject* parentObject, const ManagedSerializableFieldInfoPtr& fieldInfo)
	{
		MonoString* monoStrName = MonoUtil::wstringToMono(MonoManager::instance().getDomain(), toWString(fieldInfo->mName));
		MonoType* monoInternalType = mono_class_get_type(fieldInfo->mTypeInfo->getMonoClass());
		MonoReflectionType* internalType = mono_type_get_object(MonoManager::instance().getDomain(), monoInternalType);
		UINT32 fieldFlags = (UINT32)fieldInfo->mFlags;

		void* params[4] = { parentObject, monoStrName, &fieldFlags, internalType };
		MonoObject* managedInstance = metaData.scriptClass->createInstance(params, 4);

		ScriptSerializableField* nativeInstance = new (cm_alloc<ScriptSerializableField>()) ScriptSerializableField(fieldInfo);
		nativeInstance->createInstance(managedInstance);

		metaData.thisPtrField->setValue(managedInstance, &nativeInstance);

		return nativeInstance;
	}

	void ScriptSerializableField::internal_destroyInstance(ScriptSerializableField* nativeInstance)
	{
		nativeInstance->~ScriptSerializableField();
		cm_free(nativeInstance);
	}

	MonoObject* ScriptSerializableField::internal_getValue(ScriptSerializableField* nativeInstance, MonoObject* instance)
	{
		return nativeInstance->mFieldInfo->mMonoField->getValueBoxed(instance);
	}

	void ScriptSerializableField::internal_setValue(ScriptSerializableField* nativeInstance, MonoObject* instance, MonoObject* value)
	{
		if(mono_class_is_valuetype(mono_object_get_class(value)))
		{
			void* rawValue = mono_object_unbox(value);
			nativeInstance->mFieldInfo->mMonoField->setValue(instance, rawValue);
		}
		else
			nativeInstance->mFieldInfo->mMonoField->setValue(instance, value);
	}
}
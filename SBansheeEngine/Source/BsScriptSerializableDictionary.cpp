#include "BsScriptSerializableDictionary.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsScriptAssemblyManager.h"
#include "BsManagedSerializableObjectInfo.h"
#include "BsScriptSerializableProperty.h"

namespace BansheeEngine
{
	ScriptSerializableDictionary::ScriptSerializableDictionary(MonoObject* instance, const ManagedSerializableTypeInfoDictionaryPtr& typeInfo)
		:ScriptObject(instance), mTypeInfo(typeInfo)
	{

	}

	void ScriptSerializableDictionary::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateKeyProperty", &ScriptSerializableDictionary::internal_createKeyProperty);
		metaData.scriptClass->addInternalCall("Internal_CreateValueProperty", &ScriptSerializableDictionary::internal_createValueProperty);
	}

	ScriptSerializableDictionary* ScriptSerializableDictionary::create(const ManagedSerializableTypeInfoDictionaryPtr& typeInfo, MonoObject* object)
	{
		MonoType* monoInternalKeyType = mono_class_get_type(typeInfo->mKeyType->getMonoClass());
		MonoReflectionType* internalKeyType = mono_type_get_object(MonoManager::instance().getDomain(), monoInternalKeyType);

		MonoType* monoInternalValueType = mono_class_get_type(typeInfo->mValueType->getMonoClass());
		MonoReflectionType* internalValueType = mono_type_get_object(MonoManager::instance().getDomain(), monoInternalValueType);

		void* params[3] = { object, internalKeyType, internalValueType };
		MonoObject* managedInstance = metaData.scriptClass->createInstance(params, 3);

		ScriptSerializableDictionary* nativeInstance = new (bs_alloc<ScriptSerializableDictionary>()) ScriptSerializableDictionary(managedInstance, typeInfo);

		return nativeInstance;
	}

	MonoObject* ScriptSerializableDictionary::internal_createKeyProperty(ScriptSerializableDictionary* nativeInstance)
	{
		ScriptSerializableProperty* newProperty = ScriptSerializableProperty::create(nativeInstance->mTypeInfo->mKeyType);

		return newProperty->getManagedInstance();
	}

	MonoObject* ScriptSerializableDictionary::internal_createValueProperty(ScriptSerializableDictionary* nativeInstance)
	{
		ScriptSerializableProperty* newProperty = ScriptSerializableProperty::create(nativeInstance->mTypeInfo->mValueType);

		return newProperty->getManagedInstance();
	}
}
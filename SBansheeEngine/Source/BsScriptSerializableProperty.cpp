#include "BsScriptSerializableProperty.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsManagedSerializableObjectInfo.h"
#include "BsScriptSerializableObject.h"
#include "BsScriptSerializableArray.h"

namespace BansheeEngine
{
	ScriptSerializableProperty::ScriptSerializableProperty(const ManagedSerializableTypeInfoPtr& typeInfo)
		:mTypeInfo(typeInfo)
	{

	}

	void ScriptSerializableProperty::initMetaData()
	{
		metaData = ScriptMeta(BansheeEngineAssemblyName, "BansheeEngine", "SerializableProperty", &ScriptSerializableProperty::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void ScriptSerializableProperty::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptSerializableProperty::internal_destroyInstance);
		metaData.scriptClass->addInternalCall("Internal_CreateObject", &ScriptSerializableProperty::internal_createObject);
		metaData.scriptClass->addInternalCall("Internal_CreateArray", &ScriptSerializableProperty::internal_createArray);
	}

	ScriptSerializableProperty* ScriptSerializableProperty::create(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		MonoObject* managedInstance = metaData.scriptClass->createInstance();

		ScriptSerializableProperty* nativeInstance = new (cm_alloc<ScriptSerializableProperty>()) ScriptSerializableProperty(typeInfo);
		nativeInstance->createInstance(managedInstance);

		metaData.thisPtrField->setValue(managedInstance, &nativeInstance);

		return nativeInstance;
	}

	MonoObject* ScriptSerializableProperty::internal_createObject(ScriptSerializableProperty* nativeInstance, MonoObject* object)
	{
		ScriptSerializableObject* newObject = ScriptSerializableObject::create(nativeInstance->mTypeInfo, object);

		return newObject->getManagedInstance();
	}

	MonoObject* ScriptSerializableProperty::internal_createArray(ScriptSerializableProperty* nativeInstance, MonoObject* object)
	{
		ManagedSerializableTypeInfoArrayPtr arrayTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoArray>(nativeInstance->mTypeInfo);
		ScriptSerializableArray* newObject = ScriptSerializableArray::create(arrayTypeInfo, object);

		return newObject->getManagedInstance();
	}

	void ScriptSerializableProperty::internal_destroyInstance(ScriptSerializableProperty* nativeInstance)
	{
		nativeInstance->~ScriptSerializableProperty();
		cm_free(nativeInstance);
	}
}
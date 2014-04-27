#include "BsScriptSerializableArray.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsRuntimeScriptObjects.h"
#include "BsManagedSerializableObjectInfo.h"
#include "BsScriptSerializableProperty.h"

namespace BansheeEngine
{
	ScriptSerializableArray::ScriptSerializableArray(const ManagedSerializableTypeInfoArrayPtr& typeInfo)
		:mTypeInfo(typeInfo)
	{

	}

	void ScriptSerializableArray::initMetaData()
	{
		metaData = ScriptMeta(BansheeEngineAssemblyName, "BansheeEngine", "SerializableArray", &ScriptSerializableArray::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void ScriptSerializableArray::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptSerializableArray::internal_destroyInstance);
		metaData.scriptClass->addInternalCall("Internal_CreateProperty", &ScriptSerializableArray::internal_createProperty);
	}

	ScriptSerializableArray* ScriptSerializableArray::create(const ManagedSerializableTypeInfoArrayPtr& typeInfo, MonoObject* object)
	{
		ScriptSerializableArray* nativeInstance = new (cm_alloc<ScriptSerializableArray>()) ScriptSerializableArray(typeInfo);

		MonoType* monoInternalElementType = mono_class_get_type(typeInfo->mElementType->getMonoClass());
		MonoReflectionType* internalElementType = mono_type_get_object(MonoManager::instance().getDomain(), monoInternalElementType);

		void* params[2] = { object, internalElementType };
		MonoObject* managedInstance = metaData.scriptClass->createInstance(params, 2);

		nativeInstance->createInstance(managedInstance);

		metaData.thisPtrField->setValue(managedInstance, &nativeInstance);

		return nativeInstance;
	}

	MonoObject* ScriptSerializableArray::internal_createProperty(ScriptSerializableArray* nativeInstance)
	{
		ScriptSerializableProperty* newProperty = ScriptSerializableProperty::create(nativeInstance->mTypeInfo->mElementType);

		return newProperty->getManagedInstance();
	}

	void ScriptSerializableArray::internal_destroyInstance(ScriptSerializableArray* nativeInstance)
	{
		nativeInstance->~ScriptSerializableArray();
		cm_free(nativeInstance);
	}
}
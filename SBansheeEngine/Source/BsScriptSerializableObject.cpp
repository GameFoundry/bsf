#include "BsScriptSerializableObject.h"
#include "BsScriptSerializableField.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsManagedSerializableObject.h"
#include "BsManagedSerializableObjectInfo.h"

namespace BansheeEngine
{
	MonoField* ScriptSerializableObject::FieldsField = nullptr;

	ScriptSerializableObject::ScriptSerializableObject()
	{

	}

	void ScriptSerializableObject::initMetaData()
	{
		metaData = ScriptMeta(BansheeEngineAssemblyName, "BansheeEngine", "SerializableObject", &ScriptSerializableObject::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void ScriptSerializableObject::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptSerializableObject::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptSerializableObject::internal_destroyInstance);

		FieldsField = metaData.scriptClass->getField("_fields");
	}

	void ScriptSerializableObject::internal_createInstance(MonoObject* instance, MonoObject* object)
	{
		ScriptSerializableObject* nativeInstance = new (cm_alloc<ScriptSerializableObject>()) ScriptSerializableObject();
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, &nativeInstance);

		ManagedSerializableObjectPtr serializableObject = ManagedSerializableObject::create(object);
		if(serializableObject == nullptr) // Object is not serializable
			return;

		ManagedSerializableObjectInfoPtr objInfo = serializableObject->getObjectInfo();

		::MonoClass* serializableFieldClass = ScriptSerializableField::getMetaData()->scriptClass->_getInternalClass();

		MonoArray* serializableFieldArray = mono_array_new(MonoManager::instance().getDomain(), 
			serializableFieldClass, (UINT32)objInfo->mFields.size());

		UINT32 i = 0;
		for(auto& field : objInfo->mFields)
		{
			ScriptSerializableField* serializableField = ScriptSerializableField::create(instance, field.second);
			MonoObject* fieldManagedInstance = serializableField->getManagedInstance();

			void* elemAddr = mono_array_addr_with_size(serializableFieldArray, sizeof(MonoObject*), i);
			memcpy(elemAddr, &fieldManagedInstance, sizeof(MonoObject*));

			i++;
		}

		FieldsField->setValue(instance, serializableFieldArray);
	}

	void ScriptSerializableObject::internal_destroyInstance(ScriptSerializableObject* nativeInstance)
	{
		nativeInstance->~ScriptSerializableObject();
		cm_free(nativeInstance);
	}
}
#include "BsScriptSerializableArrayInfo.h"
#include "BsScriptSerializableField.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsRuntimeScriptObjects.h"
#include "BsManagedSerializableArray.h"
#include "BsManagedSerializableObjectInfo.h"

namespace BansheeEngine
{
	MonoField* ScriptSerializableArrayInfo::ElementTypeField = nullptr;

	ScriptSerializableArrayInfo::ScriptSerializableArrayInfo()
	{

	}

	void ScriptSerializableArrayInfo::initMetaData()
	{
		metaData = ScriptMeta(BansheeEngineAssemblyName, "BansheeEngine", "SerializableArrayInfo", &ScriptSerializableArrayInfo::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void ScriptSerializableArrayInfo::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptSerializableArrayInfo::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptSerializableArrayInfo::internal_destroyInstance);

		ElementTypeField = metaData.scriptClass->getField("elementType");
	}

	void ScriptSerializableArrayInfo::internal_createInstance(MonoObject* instance, MonoObject* object)
	{
		ScriptSerializableArrayInfo* nativeInstance = new (cm_alloc<ScriptSerializableArrayInfo>()) ScriptSerializableArrayInfo();
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, &nativeInstance);



		//MonoManager::instance().findClass(mono_object_get_class(object));

		//RuntimeScriptObjects::instance().determineType()

		//ManagedSerializableArrayPtr serializableArray = ManagedSerializableArray::create(object);
		//if(serializableObject == nullptr) // Object is not serializable
		//	return;

		//ManagedSerializableObjectInfoPtr objInfo = serializableObject->getObjectInfo();

		//::MonoClass* serializableFieldClass = ScriptSerializableField::getMetaData()->scriptClass->_getInternalClass();

		//MonoArray* serializableFieldArray = mono_array_new(MonoManager::instance().getDomain(), 
		//	serializableFieldClass, (UINT32)objInfo->mFields.size());

		//UINT32 i = 0;
		//for(auto& field : objInfo->mFields)
		//{
		//	ScriptSerializableField* serializableField = ScriptSerializableField::create(instance, field.second);
		//	MonoObject* fieldManagedInstance = serializableField->getManagedInstance();

		//	void* elemAddr = mono_array_addr_with_size(serializableFieldArray, sizeof(MonoObject*), i);
		//	memcpy(elemAddr, &fieldManagedInstance, sizeof(MonoObject*));

		//	i++;
		//}

		//FieldsField->setValue(instance, serializableFieldArray);
	}

	void ScriptSerializableArrayInfo::internal_destroyInstance(ScriptSerializableArrayInfo* nativeInstance)
	{
		nativeInstance->~ScriptSerializableArrayInfo();
		cm_free(nativeInstance);
	}
}
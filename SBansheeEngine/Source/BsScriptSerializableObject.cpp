#include "BsScriptSerializableObject.h"
#include "BsScriptSerializableField.h"
#include "BsRuntimeScriptObjects.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"

namespace BansheeEngine
{
	MonoField* ScriptSerializableObject::FieldsField = nullptr;

	ScriptSerializableObject::ScriptSerializableObject(const ManagedSerializableTypeInfoPtr& typeInfo)
		:mTypeInfo(typeInfo)
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

	ScriptSerializableObject* ScriptSerializableObject::create(const ManagedSerializableTypeInfoPtr& typeInfo, MonoObject* object)
	{
		ManagedSerializableTypeInfoObject* objTypeInfo = static_cast<ManagedSerializableTypeInfoObject*>(typeInfo.get());

		ManagedSerializableObjectInfoPtr objInfo;
		RuntimeScriptObjects::instance().getSerializableObjectInfo(objTypeInfo->mTypeNamespace, objTypeInfo->mTypeName, objInfo);

		MonoObject* managedInstance = metaData.scriptClass->createInstance();

		return createInternal(managedInstance, objInfo);
	}

	void ScriptSerializableObject::internal_createInstance(MonoObject* instance, MonoReflectionType* type, MonoObject* object)
	{
		MonoType* internalType = mono_reflection_type_get_type(type);
		::MonoClass* monoClass = mono_type_get_class(internalType);
		String elementNs = mono_class_get_namespace(monoClass);
		String elementTypeName = mono_class_get_name(monoClass);

		ManagedSerializableObjectInfoPtr objInfo;
		RuntimeScriptObjects::instance().getSerializableObjectInfo(elementNs, elementTypeName, objInfo);

		createInternal(instance, objInfo);
	}

	ScriptSerializableObject* ScriptSerializableObject::createInternal(MonoObject* instance, const ManagedSerializableObjectInfoPtr& objInfo)
	{
		ManagedSerializableTypeInfoPtr typeInfo;
		if(objInfo != nullptr)
			typeInfo = objInfo->mTypeInfo;

		ScriptSerializableObject* nativeInstance = new (cm_alloc<ScriptSerializableObject>()) ScriptSerializableObject(typeInfo);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, &nativeInstance);

		if(objInfo != nullptr)
		{
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

		return nativeInstance;
	}

	void ScriptSerializableObject::internal_destroyInstance(ScriptSerializableObject* nativeInstance)
	{
		nativeInstance->~ScriptSerializableObject();
		cm_free(nativeInstance);
	}
}
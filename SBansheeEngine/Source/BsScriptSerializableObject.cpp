#include "BsScriptSerializableObject.h"
#include "BsScriptSerializableField.h"
#include "BsScriptAssemblyManager.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"

namespace BansheeEngine
{
	MonoField* ScriptSerializableObject::FieldsField = nullptr;

	ScriptSerializableObject::ScriptSerializableObject(MonoObject* instance, const ManagedSerializableTypeInfoPtr& typeInfo)
		:ScriptObject(instance), mTypeInfo(typeInfo)
	{

	}

	void ScriptSerializableObject::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptSerializableObject::internal_createInstance);

		FieldsField = metaData.scriptClass->getField("_fields");
	}

	ScriptSerializableObject* ScriptSerializableObject::create(const ManagedSerializableTypeInfoPtr& typeInfo, MonoObject* object)
	{
		MonoType* monoInternalElementType = mono_class_get_type(typeInfo->getMonoClass());
		MonoReflectionType* internalElementType = mono_type_get_object(MonoManager::instance().getDomain(), monoInternalElementType);

		void* params[2] = { internalElementType, object };
		MonoObject* managedInstance = metaData.scriptClass->createInstance(params, 2);

		// Managed constructor will call back to native which will create ScriptSerializableObject instance,
		// and we can now just retrieve it.
		return ScriptSerializableObject::toNative(managedInstance);
	}

	void ScriptSerializableObject::internal_createInstance(MonoObject* instance, MonoReflectionType* type, MonoObject* object)
	{
		MonoType* internalType = mono_reflection_type_get_type(type);
		::MonoClass* monoClass = mono_type_get_class(internalType);

		String elementNs;
		String elementTypeName;
		MonoUtil::getClassName(monoClass, elementNs, elementTypeName);

		ManagedSerializableObjectInfoPtr objInfo;
		ScriptAssemblyManager::instance().getSerializableObjectInfo(elementNs, elementTypeName, objInfo);

		createInternal(instance, objInfo);
	}

	ScriptSerializableObject* ScriptSerializableObject::createInternal(MonoObject* instance, const ManagedSerializableObjectInfoPtr& objInfo)
	{
		ManagedSerializableTypeInfoPtr typeInfo;
		if(objInfo != nullptr)
			typeInfo = objInfo->mTypeInfo;

		ScriptSerializableObject* nativeInstance = new (bs_alloc<ScriptSerializableObject>()) ScriptSerializableObject(instance, typeInfo);

		if(objInfo != nullptr)
		{
			::MonoClass* serializableFieldClass = ScriptSerializableField::getMetaData()->scriptClass->_getInternalClass();

			MonoArray* serializableFieldArray = mono_array_new(MonoManager::instance().getDomain(), 
				serializableFieldClass, (UINT32)objInfo->mFields.size());

			Vector<ManagedSerializableFieldInfoPtr> sortedFields((UINT32)objInfo->mFields.size());
			UINT32 i = 0;
			for (auto& fieldPair : objInfo->mFields)
			{
				sortedFields[i] = fieldPair.second;
				i++;
			}

			std::sort(sortedFields.begin(), sortedFields.end(), 
				[&](const ManagedSerializableFieldInfoPtr& x, const ManagedSerializableFieldInfoPtr& y) 
			{
				return x->mFieldId < y->mFieldId;
			});

			i = 0;
			for (auto& field : sortedFields)
			{
				ScriptSerializableField* serializableField = ScriptSerializableField::create(instance, field);
				MonoObject* fieldManagedInstance = serializableField->getManagedInstance();

				void* elemAddr = mono_array_addr_with_size(serializableFieldArray, sizeof(MonoObject*), i);
				memcpy(elemAddr, &fieldManagedInstance, sizeof(MonoObject*));

				i++;
			}

			FieldsField->setValue(instance, serializableFieldArray);
		}

		return nativeInstance;
	}
}
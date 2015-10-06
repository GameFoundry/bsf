#include "BsScriptSerializableProperty.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsManagedSerializableObjectInfo.h"
#include "BsScriptSerializableObject.h"
#include "BsScriptSerializableArray.h"
#include "BsScriptSerializableList.h"
#include "BsScriptSerializableDictionary.h"
#include "BsManagedSerializableObject.h"
#include "BsManagedSerializableArray.h"
#include "BsManagedSerializableList.h"
#include "BsManagedSerializableDictionary.h"
#include "BsManagedSerializableField.h"
#include "BsMemorySerializer.h"

namespace BansheeEngine
{
	ScriptSerializableProperty::ScriptSerializableProperty(MonoObject* instance, const ManagedSerializableTypeInfoPtr& typeInfo)
		:ScriptObject(instance), mTypeInfo(typeInfo)
	{

	}

	void ScriptSerializableProperty::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateObject", &ScriptSerializableProperty::internal_createObject);
		metaData.scriptClass->addInternalCall("Internal_CreateArray", &ScriptSerializableProperty::internal_createArray);
		metaData.scriptClass->addInternalCall("Internal_CreateList", &ScriptSerializableProperty::internal_createList);
		metaData.scriptClass->addInternalCall("Internal_CreateDictionary", &ScriptSerializableProperty::internal_createDictionary);
		metaData.scriptClass->addInternalCall("Internal_CreateManagedObjectInstance", &ScriptSerializableProperty::internal_createManagedObjectInstance);
		metaData.scriptClass->addInternalCall("Internal_CreateManagedArrayInstance", &ScriptSerializableProperty::internal_createManagedArrayInstance);
		metaData.scriptClass->addInternalCall("Internal_CreateManagedListInstance", &ScriptSerializableProperty::internal_createManagedListInstance);
		metaData.scriptClass->addInternalCall("Internal_CreateManagedDictionaryInstance", &ScriptSerializableProperty::internal_createManagedDictionaryInstance);
	}

	ScriptSerializableProperty* ScriptSerializableProperty::create(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		MonoObject* managedInstance = metaData.scriptClass->createInstance();

		ScriptSerializableProperty* nativeInstance = new (bs_alloc<ScriptSerializableProperty>()) ScriptSerializableProperty(managedInstance, typeInfo);

		return nativeInstance;
	}

	MonoObject* ScriptSerializableProperty::internal_createObject(ScriptSerializableProperty* nativeInstance)
	{
		ScriptSerializableObject* newObject = ScriptSerializableObject::create(nativeInstance);

		return newObject->getManagedInstance();
	}

	MonoObject* ScriptSerializableProperty::internal_createArray(ScriptSerializableProperty* nativeInstance)
	{
		ScriptSerializableArray* newObject = ScriptSerializableArray::create(nativeInstance);

		return newObject->getManagedInstance();
	}

	MonoObject* ScriptSerializableProperty::internal_createList(ScriptSerializableProperty* nativeInstance)
	{
		ScriptSerializableList* newObject = ScriptSerializableList::create(nativeInstance);

		return newObject->getManagedInstance();
	}

	MonoObject* ScriptSerializableProperty::internal_createDictionary(ScriptSerializableProperty* nativeInstance)
	{
		ScriptSerializableDictionary* newObject = ScriptSerializableDictionary::create(nativeInstance);

		return newObject->getManagedInstance();
	}

	MonoObject* ScriptSerializableProperty::internal_createManagedObjectInstance(ScriptSerializableProperty* nativeInstance)
	{
		ManagedSerializableTypeInfoObjectPtr objectTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoObject>(nativeInstance->mTypeInfo);
		return ManagedSerializableObject::createManagedInstance(objectTypeInfo);
	}

	MonoObject* ScriptSerializableProperty::internal_createManagedArrayInstance(ScriptSerializableProperty* nativeInstance, MonoArray* sizes)
	{
		Vector<UINT32> nativeSizes;
		UINT32 arrayLen = (UINT32)mono_array_length(sizes);
		for (UINT32 i = 0; i < arrayLen; i++)
			nativeSizes.push_back(mono_array_get(sizes, UINT32, i));

		ManagedSerializableTypeInfoArrayPtr arrayTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoArray>(nativeInstance->mTypeInfo);
		return ManagedSerializableArray::createManagedInstance(arrayTypeInfo, nativeSizes);
	}

	MonoObject* ScriptSerializableProperty::internal_createManagedListInstance(ScriptSerializableProperty* nativeInstance, int size)
	{
		ManagedSerializableTypeInfoListPtr listTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoList>(nativeInstance->mTypeInfo);
		return ManagedSerializableList::createManagedInstance(listTypeInfo, size);
	}

	MonoObject* ScriptSerializableProperty::internal_createManagedDictionaryInstance(ScriptSerializableProperty* nativeInstance)
	{
		ManagedSerializableTypeInfoDictionaryPtr dictTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoDictionary>(nativeInstance->mTypeInfo);
		return ManagedSerializableDictionary::createManagedInstance(dictTypeInfo);
	}
}
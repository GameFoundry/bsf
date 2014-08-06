#include "BsScriptSerializableProperty.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsManagedSerializableObjectInfo.h"
#include "BsScriptSerializableObject.h"
#include "BsScriptSerializableArray.h"
#include "BsManagedSerializableObject.h"
#include "BsManagedSerializableArray.h"
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
		metaData.scriptClass->addInternalCall("Internal_CreateManagedObjectInstance", &ScriptSerializableProperty::internal_createManagedObjectInstance);
		metaData.scriptClass->addInternalCall("Internal_CreateManagedArrayInstance", &ScriptSerializableProperty::internal_createManagedArrayInstance);
		metaData.scriptClass->addInternalCall("Internal_CloneManagedInstance", &ScriptSerializableProperty::internal_cloneManagedInstance);
	}

	ScriptSerializableProperty* ScriptSerializableProperty::create(const ManagedSerializableTypeInfoPtr& typeInfo)
	{
		MonoObject* managedInstance = metaData.scriptClass->createInstance();

		ScriptSerializableProperty* nativeInstance = new (bs_alloc<ScriptSerializableProperty>()) ScriptSerializableProperty(managedInstance, typeInfo);

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

	MonoObject* ScriptSerializableProperty::internal_cloneManagedInstance(ScriptSerializableProperty* nativeInstance, MonoObject* original)
	{
		ManagedSerializableFieldDataPtr data = ManagedSerializableFieldData::create(nativeInstance->mTypeInfo, original);

		MemorySerializer ms;

		// Note: This code unnecessarily encodes to binary and decodes from it. I could have added a specialized clone method that does it directly,
		// but didn't feel the extra code was justified.
		UINT32 size = 0;
		UINT8* encodedData = ms.encode(data.get(), size);
		ManagedSerializableFieldDataPtr clonedData = std::static_pointer_cast<ManagedSerializableFieldData>(ms.decode(encodedData, size));

		return clonedData->getValueBoxed(nativeInstance->mTypeInfo);
	}
}
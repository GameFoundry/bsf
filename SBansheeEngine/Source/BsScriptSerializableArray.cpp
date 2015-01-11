#include "BsScriptSerializableArray.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsScriptAssemblyManager.h"
#include "BsManagedSerializableObjectInfo.h"
#include "BsScriptSerializableProperty.h"

namespace BansheeEngine
{
	ScriptSerializableArray::ScriptSerializableArray(MonoObject* instance, const ManagedSerializableTypeInfoArrayPtr& typeInfo)
		:ScriptObject(instance), mTypeInfo(typeInfo)
	{

	}

	void ScriptSerializableArray::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateProperty", &ScriptSerializableArray::internal_createProperty);
	}

	ScriptSerializableArray* ScriptSerializableArray::create(const ManagedSerializableTypeInfoArrayPtr& typeInfo, MonoObject* object)
	{
		MonoType* monoInternalElementType = mono_class_get_type(typeInfo->mElementType->getMonoClass());
		MonoReflectionType* internalElementType = mono_type_get_object(MonoManager::instance().getDomain(), monoInternalElementType);

		void* params[2] = { object, internalElementType };
		MonoObject* managedInstance = metaData.scriptClass->createInstance(params, 2);

		ScriptSerializableArray* nativeInstance = new (bs_alloc<ScriptSerializableArray>()) ScriptSerializableArray(managedInstance, typeInfo);

		return nativeInstance;
	}

	MonoObject* ScriptSerializableArray::internal_createProperty(ScriptSerializableArray* nativeInstance)
	{
		ScriptSerializableProperty* newProperty = ScriptSerializableProperty::create(nativeInstance->mTypeInfo->mElementType);

		return newProperty->getManagedInstance();
	}
}
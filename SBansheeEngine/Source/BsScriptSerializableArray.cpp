//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
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

	ScriptSerializableArray* ScriptSerializableArray::create(const ScriptSerializableProperty* parentProperty)
	{
		ManagedSerializableTypeInfoArrayPtr arrayTypeInfo = std::static_pointer_cast<ManagedSerializableTypeInfoArray>(parentProperty->getTypeInfo());

		MonoType* monoInternalElementType = mono_class_get_type(arrayTypeInfo->mElementType->getMonoClass());
		MonoReflectionType* internalElementType = mono_type_get_object(MonoManager::instance().getDomain(), monoInternalElementType);

		void* params[2] = { internalElementType, parentProperty->getManagedInstance() };
		MonoObject* managedInstance = metaData.scriptClass->createInstance(params, 2);

		ScriptSerializableArray* nativeInstance = new (bs_alloc<ScriptSerializableArray>()) ScriptSerializableArray(managedInstance, arrayTypeInfo);

		return nativeInstance;
	}

	MonoObject* ScriptSerializableArray::internal_createProperty(ScriptSerializableArray* nativeInstance)
	{
		ScriptSerializableProperty* newProperty = ScriptSerializableProperty::create(nativeInstance->mTypeInfo->mElementType);

		return newProperty->getManagedInstance();
	}
}
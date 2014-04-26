#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSerializableProperty : public ScriptObject<ScriptSerializableProperty>
	{
	public:
		static void initMetaData();

		static ScriptSerializableProperty* create(const ManagedSerializableTypeInfoPtr& typeInfo);

	private:
		static void internal_destroyInstance(ScriptSerializableProperty* nativeInstance);
		static MonoObject* internal_createObject(ScriptSerializableProperty* nativeInstance, MonoObject* object);
		static MonoObject* internal_createArray(ScriptSerializableProperty* nativeInstance, MonoObject* object);

		static void initRuntimeData();

		ScriptSerializableProperty(const ManagedSerializableTypeInfoPtr& typeInfo);
		~ScriptSerializableProperty() {}

		ManagedSerializableTypeInfoPtr mTypeInfo;
	};
}
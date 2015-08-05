#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSerializableProperty : public ScriptObject<ScriptSerializableProperty>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "SerializableProperty")

		static ScriptSerializableProperty* create(const ManagedSerializableTypeInfoPtr& typeInfo);

		ManagedSerializableTypeInfoPtr getTypeInfo() const { return mTypeInfo; }

		~ScriptSerializableProperty() {}

	private:
		static MonoObject* internal_createObject(ScriptSerializableProperty* nativeInstance);
		static MonoObject* internal_createArray(ScriptSerializableProperty* nativeInstance);
		static MonoObject* internal_createList(ScriptSerializableProperty* nativeInstance);
		static MonoObject* internal_createDictionary(ScriptSerializableProperty* nativeInstance);

		static MonoObject* internal_createManagedObjectInstance(ScriptSerializableProperty* nativeInstance);
		static MonoObject* internal_createManagedArrayInstance(ScriptSerializableProperty* nativeInstance, MonoArray* sizes);
		static MonoObject* internal_createManagedListInstance(ScriptSerializableProperty* nativeInstance, int size);
		static MonoObject* internal_createManagedDictionaryInstance(ScriptSerializableProperty* nativeInstance);

		static MonoObject* internal_cloneManagedInstance(ScriptSerializableProperty* nativeInstance, MonoObject* original);

		ScriptSerializableProperty(MonoObject* instance, const ManagedSerializableTypeInfoPtr& typeInfo);

		ManagedSerializableTypeInfoPtr mTypeInfo;
	};
}
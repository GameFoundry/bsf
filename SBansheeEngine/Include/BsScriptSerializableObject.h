#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSerializableObject : public ScriptObject<ScriptSerializableObject>
	{
	public:
		static void initMetaData();

		static ScriptSerializableObject* create(const ManagedSerializableTypeInfoPtr& typeInfo, MonoObject* object);

	private:
		static void internal_createInstance(MonoObject* instance, MonoReflectionType* type, MonoObject* object);
		static void internal_destroyInstance(ScriptSerializableObject* nativeInstance);

		static void initRuntimeData();

		static ScriptSerializableObject* createInternal(MonoObject* instance, const ManagedSerializableObjectInfoPtr& objInfo);

		ScriptSerializableObject(const ManagedSerializableTypeInfoPtr& typeInfo);
		~ScriptSerializableObject() {}

		ManagedSerializableTypeInfoPtr mTypeInfo;

		static MonoField* FieldsField;
	};
}
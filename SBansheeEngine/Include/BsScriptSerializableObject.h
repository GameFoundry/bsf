#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSerializableObject : public ScriptObject<ScriptSerializableObject>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "SerializableObject")

		static ScriptSerializableObject* create(ScriptSerializableProperty* parentProperty, MonoObject* object);

	private:
		static void internal_createInstance(MonoObject* instance, MonoReflectionType* type);

		static ScriptSerializableObject* createInternal(MonoObject* instance, const ManagedSerializableObjectInfoPtr& objInfo);

		ScriptSerializableObject(MonoObject* instance, const ManagedSerializableTypeInfoPtr& typeInfo);

		ManagedSerializableTypeInfoPtr mTypeInfo;

		static MonoField* FieldsField;
	};
}
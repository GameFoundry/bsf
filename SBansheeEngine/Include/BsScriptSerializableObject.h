#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSerializableObject : public ScriptObject<ScriptSerializableObject>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "SerializableObject")

		static ScriptSerializableObject* create(const ManagedSerializableTypeInfoPtr& typeInfo, MonoObject* object);

	private:
		static void internal_createInstance(MonoObject* instance, MonoReflectionType* type, MonoObject* object);

		static ScriptSerializableObject* createInternal(MonoObject* instance, const ManagedSerializableObjectInfoPtr& objInfo);

		ScriptSerializableObject(MonoObject* instance, const ManagedSerializableTypeInfoPtr& typeInfo);

		ManagedSerializableTypeInfoPtr mTypeInfo;

		static MonoField* FieldsField;
	};
}
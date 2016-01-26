#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSerializableField : public ScriptObject<ScriptSerializableField>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "SerializableField")

		static ScriptSerializableField* create(MonoObject* parentObject, const ManagedSerializableFieldInfoPtr& fieldInfo);

	private:
		static MonoObject* internal_createProperty(ScriptSerializableField* nativeInstance);
		static MonoObject* internal_getValue(ScriptSerializableField* nativeInstance, MonoObject* instance);
		static void internal_setValue(ScriptSerializableField* nativeInstance, MonoObject* instance, MonoObject* value);

		static UINT32 nativeToManagedFieldType(const ManagedSerializableTypeInfoPtr& typeInfo);

		ScriptSerializableField(MonoObject* instance, const ManagedSerializableFieldInfoPtr& fieldInfo);

		ManagedSerializableFieldInfoPtr mFieldInfo;
	};
}
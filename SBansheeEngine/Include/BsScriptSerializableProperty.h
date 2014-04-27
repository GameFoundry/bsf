#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSerializableProperty : public ScriptObject<ScriptSerializableProperty>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "SerializableProperty")

		static ScriptSerializableProperty* create(const ManagedSerializableTypeInfoPtr& typeInfo);

		~ScriptSerializableProperty() {}

	private:
		static MonoObject* internal_createObject(ScriptSerializableProperty* nativeInstance, MonoObject* object);
		static MonoObject* internal_createArray(ScriptSerializableProperty* nativeInstance, MonoObject* object);

		ScriptSerializableProperty(MonoObject* instance, const ManagedSerializableTypeInfoPtr& typeInfo);

		ManagedSerializableTypeInfoPtr mTypeInfo;
	};
}
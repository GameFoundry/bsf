#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSerializableArray : public ScriptObject<ScriptSerializableArray>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "SerializableArray")

		static ScriptSerializableArray* create(const ManagedSerializableTypeInfoArrayPtr& typeInfo, MonoObject* object);

	private:
		static MonoObject* internal_createProperty(ScriptSerializableArray* nativeInstance);

		ScriptSerializableArray(MonoObject* instance, const ManagedSerializableTypeInfoArrayPtr& typeInfo);

		ManagedSerializableTypeInfoArrayPtr mTypeInfo;
	};
}
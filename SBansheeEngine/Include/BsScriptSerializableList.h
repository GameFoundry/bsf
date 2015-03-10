#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSerializableList : public ScriptObject<ScriptSerializableList>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "SerializableList")

			static ScriptSerializableList* create(const ManagedSerializableTypeInfoListPtr& typeInfo, MonoObject* object);

	private:
		static MonoObject* internal_createProperty(ScriptSerializableList* nativeInstance);

		ScriptSerializableList(MonoObject* instance, const ManagedSerializableTypeInfoListPtr& typeInfo);

		ManagedSerializableTypeInfoListPtr mTypeInfo;
	};
}
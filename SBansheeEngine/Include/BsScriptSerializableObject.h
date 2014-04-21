#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSerializableObject : public ScriptObject<ScriptSerializableObject>
	{
	public:
		static void initMetaData();

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* object);
		static void internal_destroyInstance(ScriptSerializableObject* nativeInstance);

		static void initRuntimeData();

		ScriptSerializableObject();
		~ScriptSerializableObject() {}

		static MonoField* FieldsField;
	};
}
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSerializableArrayInfo : public ScriptObject<ScriptSerializableArrayInfo>
	{
	public:
		static void initMetaData();

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* object);
		static void internal_destroyInstance(ScriptSerializableArrayInfo* nativeInstance);

		static void initRuntimeData();

		ScriptSerializableArrayInfo();
		~ScriptSerializableArrayInfo() {}

		static MonoField* ElementTypeField;
	};
}
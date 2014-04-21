#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSerializableField : public ScriptObject<ScriptSerializableField>
	{
	public:
		static void initMetaData();

		static ScriptSerializableField* create(MonoObject* parentObject, const ManagedSerializableFieldInfoPtr& fieldInfo);

	private:
		static void internal_destroyInstance(ScriptSerializableField* nativeInstance);

		static MonoObject* internal_getValue(ScriptSerializableField* nativeInstance);
		static void internal_setValue(ScriptSerializableField* nativeInstance, MonoObject* value);

		static void initRuntimeData();

		static UINT32 nativeToManagedFieldType(const ManagedSerializableTypeInfoPtr& typeInfo);

		ScriptSerializableField(const ManagedSerializableFieldInfoPtr& fieldInfo);
		~ScriptSerializableField() {}

		ManagedSerializableFieldInfoPtr mFieldInfo;
	};
}
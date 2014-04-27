#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSerializableArray : public ScriptObject<ScriptSerializableArray>
	{
	public:
		static void initMetaData();

		static ScriptSerializableArray* create(const ManagedSerializableTypeInfoArrayPtr& typeInfo, MonoObject* object);

	private:
		static void internal_destroyInstance(ScriptSerializableArray* nativeInstance);
		static MonoObject* internal_createProperty(ScriptSerializableArray* nativeInstance);

		static void initRuntimeData();

		ScriptSerializableArray(const ManagedSerializableTypeInfoArrayPtr& typeInfo);
		~ScriptSerializableArray() {}

		ManagedSerializableTypeInfoArrayPtr mTypeInfo;
	};
}
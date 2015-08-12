#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for ManagedSerializableFieldInfo.
	 */
	class BS_SCR_BE_EXPORT ScriptSerializableField : public ScriptObject<ScriptSerializableField>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "SerializableField")

		/**
		 * @brief	Creates a new serializable field interop object that references a specific field on
		 *			a specific object.
		 *
		 * @param	parentObject	Instance of the parent object the field belongs to.
		 * @param	fieldInfo		Information about the field. Caller must ensure the type
		 *							matches the type of the provided parent object.
		 */
		static ScriptSerializableField* create(MonoObject* parentObject, const ManagedSerializableFieldInfoPtr& fieldInfo);

	private:
		ScriptSerializableField(MonoObject* instance, const ManagedSerializableFieldInfoPtr& fieldInfo);

		ManagedSerializableFieldInfoPtr mFieldInfo;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static MonoObject* internal_createProperty(ScriptSerializableField* nativeInstance);
		static MonoObject* internal_getValue(ScriptSerializableField* nativeInstance, MonoObject* instance);
		static void internal_setValue(ScriptSerializableField* nativeInstance, MonoObject* instance, MonoObject* value);
	};
}
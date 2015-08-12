#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for ManagedSerializableArray.
	 */
	class BS_SCR_BE_EXPORT ScriptSerializableArray : public ScriptObject<ScriptSerializableArray>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "SerializableArray")

		/**
		 * @brief	Creates a new serializable array interop object from the data in the
		 *			provided property. Caller must ensure the property references an array.
		 */
		static ScriptSerializableArray* create(const ScriptSerializableProperty* parentProperty);

	private:
		ScriptSerializableArray(MonoObject* instance, const ManagedSerializableTypeInfoArrayPtr& typeInfo);

		ManagedSerializableTypeInfoArrayPtr mTypeInfo;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static MonoObject* internal_createProperty(ScriptSerializableArray* nativeInstance);
	};
}
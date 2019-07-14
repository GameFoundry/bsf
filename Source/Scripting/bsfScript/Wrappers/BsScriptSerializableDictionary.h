//********************************* bs::framework - Copyright 2018-2019 Marko Pintera ************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace bs
{
	/** @addtogroup ScriptInteropEngine
	 *  @{
	 */

	/**	Interop class between C++ & CLR for ManagedSerializableDictionary. */
	class BS_SCR_BE_EXPORT ScriptSerializableDictionary : public ScriptObject<ScriptSerializableDictionary>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, ENGINE_NS, "SerializableDictionary")

		/**
		 * Creates a new serializable dictionary interop object from the data in the provided property. Caller must ensure
		 * the property references a System.Collections.Generic.Dictionary.
		 */
		static MonoObject* create(const ScriptSerializableProperty* native, MonoObject* managed);

	private:
		ScriptSerializableDictionary(MonoObject* instance, const SPtr<ManagedSerializableTypeInfoDictionary>& typeInfo);

		SPtr<ManagedSerializableTypeInfoDictionary> mTypeInfo;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static MonoObject* internal_createKeyProperty(ScriptSerializableDictionary* nativeInstance);
		static MonoObject* internal_createValueProperty(ScriptSerializableDictionary* nativeInstance);
	};

	/** @} */
}

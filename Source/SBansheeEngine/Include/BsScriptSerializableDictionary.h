//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for ManagedSerializableDictionary.
	 */
	class BS_SCR_BE_EXPORT ScriptSerializableDictionary : public ScriptObject<ScriptSerializableDictionary>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "SerializableDictionary")

		/**
		 * @brief	Creates a new serializable dictionary interop object from the data in the
		 *			provided property. Caller must ensure the property references a System.Collections.Generic.Dictionary.
		 */
		static ScriptSerializableDictionary* create(const ScriptSerializableProperty* parentProperty);

	private:
		ScriptSerializableDictionary(MonoObject* instance, const ManagedSerializableTypeInfoDictionaryPtr& typeInfo);

		ManagedSerializableTypeInfoDictionaryPtr mTypeInfo;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static MonoObject* internal_createKeyProperty(ScriptSerializableDictionary* nativeInstance);
		static MonoObject* internal_createValueProperty(ScriptSerializableDictionary* nativeInstance);
	};
}
//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for ManagedSerializableList.
	 */
	class BS_SCR_BE_EXPORT ScriptSerializableList : public ScriptObject<ScriptSerializableList>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "SerializableList")

		/**
		 * @brief	Creates a new serializable list interop object from the data in the
		 *			provided property. Caller must ensure the property references a System.Collections.Generic.List.
		 */
		static ScriptSerializableList* create(const ScriptSerializableProperty* parentProperty);

	private:
		ScriptSerializableList(MonoObject* instance, const ManagedSerializableTypeInfoListPtr& typeInfo);

		ManagedSerializableTypeInfoListPtr mTypeInfo;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static MonoObject* internal_createProperty(ScriptSerializableList* nativeInstance);
	};
}
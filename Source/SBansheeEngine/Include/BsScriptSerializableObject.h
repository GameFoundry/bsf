//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for ManagedSerializableObject.
	 */
	class BS_SCR_BE_EXPORT ScriptSerializableObject : public ScriptObject<ScriptSerializableObject>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "SerializableObject")

		/**
		 * @brief	Creates a new serializable object interop object from the data in the
		 *			provided property. 
		 */
		static ScriptSerializableObject* create(const ScriptSerializableProperty* parentProperty);

	private:
		ScriptSerializableObject(MonoObject* instance, const ManagedSerializableTypeInfoPtr& typeInfo);

		/**
		 * @brief	Creates a new interop object for a serializable object from an existing managed instance and
		 *			an object info structure describing the type of the managed instance.
		 *
		 * @param	instance	Managed instance the interop object will reference.
		 * @param	objInfo		Data about the type of the provided managed instance.
		 */
		static ScriptSerializableObject* createInternal(MonoObject* instance, const ManagedSerializableObjectInfoPtr& objInfo);

		ManagedSerializableTypeInfoPtr mTypeInfo;
		static MonoField* FieldsField;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_createInstance(MonoObject* instance, MonoReflectionType* type);
	};
}
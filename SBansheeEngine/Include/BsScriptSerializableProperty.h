#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for SerializableProperty (not implemented as its
	 *			own class, but instead implemented directly in the inerop object). Serializable property
	 *			wraps all types of serializable entries, whether they're object fields, array entries, dictionary
	 *			entries, etc. and offers a simple interface to access them.
	 */
	class BS_SCR_BE_EXPORT ScriptSerializableProperty : public ScriptObject<ScriptSerializableProperty>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "SerializableProperty")

		/**
		 * @brief	Creates a new managed serializable property object for the specified type.
		 *
		 * @param	typeInfo	Data about the type the property references.
		 */
		static ScriptSerializableProperty* create(const ManagedSerializableTypeInfoPtr& typeInfo);

		/**
		 * @brief	Returns the data about the type the property is referencing.
		 */
		ManagedSerializableTypeInfoPtr getTypeInfo() const { return mTypeInfo; }

		~ScriptSerializableProperty() {}

	private:
		ScriptSerializableProperty(MonoObject* instance, const ManagedSerializableTypeInfoPtr& typeInfo);

		ManagedSerializableTypeInfoPtr mTypeInfo;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_CreateInstance(MonoObject* instance, MonoReflectionType* reflType);

		static MonoObject* internal_createObject(ScriptSerializableProperty* nativeInstance);
		static MonoObject* internal_createArray(ScriptSerializableProperty* nativeInstance);
		static MonoObject* internal_createList(ScriptSerializableProperty* nativeInstance);
		static MonoObject* internal_createDictionary(ScriptSerializableProperty* nativeInstance);

		static MonoObject* internal_createManagedObjectInstance(ScriptSerializableProperty* nativeInstance);
		static MonoObject* internal_createManagedArrayInstance(ScriptSerializableProperty* nativeInstance, MonoArray* sizes);
		static MonoObject* internal_createManagedListInstance(ScriptSerializableProperty* nativeInstance, int size);
		static MonoObject* internal_createManagedDictionaryInstance(ScriptSerializableProperty* nativeInstance);
	};
}
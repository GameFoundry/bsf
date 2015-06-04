#pragma once

#include "BsMonoPrerequisites.h"
#include <mono/jit/jit.h>
#include <mono/metadata/class.h>

namespace BansheeEngine
{
	/**
	 * @brief	Returns the level of field visibility in the class.
	 */
	enum class MonoFieldVisibility
	{
		Private,
		ProtectedInternal,
		Internal,
		Protected,
		Public
	};

	/**
	 * @brief	Encapsulates information about a single Mono (i.e. managed) field
	 *			belonging to some managed class. This object also allows you to set
	 *			or retrieve values to/from specific instances containing the field.
	 */
	class BS_MONO_EXPORT MonoField
	{
	public:
		/**
		 * @brief	Returns the name of the field.
		 */
		const String& getName() const { return mName; }

		/**
		 * @brief	Returns the class representing the type of data the field holds.
		 */
		MonoClass* getType();

		/**
		 * @brief	Retrieves value currently set in the field on the specified object instance.
		 *			If field is static object instance can be null. 
		 *
		 * @note	Value will be a pointer to raw data type for value types (e.g. int, float), and a MonoObject*
		 *			for reference types.
		 */
		void getValue(MonoObject* instance, void* outValue);

		/**
		 * @brief	Retrieves value currently set in the field on the specified object instance.
		 *			If field is static object instance can be null. If returned value is a value
		 *			type it will be boxed.
		 */
		MonoObject* getValueBoxed(MonoObject* instance);

		/**
		 * @brief	Sets a value for the field on the specified object instance.
		 *			If field is static object instance can be null. 
		 *
		 * @note	Value should be a pointer to raw data type for value types (e.g. int, float), and a MonoObject*
		 *			for reference types.
		 */
		void setValue(MonoObject* instance, void* value);

		/**
		 * @brief	Checks if field has an attribute of the specified type.
		 */
		bool hasAttribute(MonoClass* monoClass);

		/**
		 * @brief	Returns an instance of an attribute of the specified type. Returns null
		 *			if the field doesn't have such an attribute.
		 */
		MonoObject* getAttribute(MonoClass* monoClass);

		/**
		 * @brief	Returns field visibility in the class.
		 */
		MonoFieldVisibility getVisibility();

		/**
		 * @brief	Query if the field is static.
		 */
		bool isStatic();

	private:
		friend class MonoClass;

		MonoField(MonoClassField* field);

		String mName;
		MonoClassField* mField;
		MonoClass* mFieldType;
	};
}
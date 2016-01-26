//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsMonoPrerequisites.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	/**
	 * @brief	Encapsulates information about a single Mono (i.e. managed) property
	 *			belonging to some managed class. This object also allows you to set
	 *			or retrieve values to/from specific instances containing the property.
	 */
	class BS_MONO_EXPORT MonoProperty
	{
	public:
		/**
		 * @brief	Returns a boxed value contained in the property in the specified instance.
		 *
		 * @param	instance	Object instance to access the property on. Can be null for static properties.
		 *
		 * @returns	A boxed value of the property.
		 */
		MonoObject* get(MonoObject* instance) const;

		/**
		 * @brief	Sets a value of the property in the specified instance. 
		 *
		 * @param	instance	Object instance to access the property on. Can be null for static properties.
		 * @param	value		Value to set on the property. For value type it should be a pointer to the 
		 *						value and for reference type it should be a pointer to MonoObject.
		 */
		void set(MonoObject* instance, void* value) const;

		/**
		 * @brief	Returns a boxed value contained in the property in the specified instance. Used for properties
		 *			with indexers.
		 *
		 * @param	instance	Object instance to access the property on. Can be null for static properties.
		 * @param	index		Index of the value to retrieve.
		 *
		 * @returns	A boxed value of the property.
		 */
		MonoObject* getIndexed(MonoObject* instance, UINT32 index) const;

		/**
		 * @brief	Sets a value of the property in the specified instance. Used for properties
		 *			with indexers.
		 *
		 * @param	instance	Object instance to access the property on. Can be null for static properties.
		 * @param	index		Index of the value to set.
		 * @param	value		Value to set on the property. For value type it should be a pointer to the 
		 *						value and for reference type it should be a pointer to MonoObject.
		 */
		void setIndexed(MonoObject* instance, UINT32 index, void* value) const;

		/**
		 * @brief	Returns the data type the property holds.
		 */
		MonoClass* getReturnType();
	private:
		friend class MonoClass;

		MonoProperty(::MonoProperty* monoProp);

		::MonoProperty* mProperty;
		::MonoMethod* mGetMethod;
		::MonoMethod* mSetMethod;
		MonoClass* mGetReturnType;
	};
}
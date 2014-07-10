//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsPrerequisitesUtil.h"
#include "BsRTTIField.h"
#include "BsIReflectable.h"

namespace BansheeEngine
{
	/**
	 * @brief	Base class containing common functionality for a reflectable pointer class field. 
	 * 			
	 * @note	Reflectable fields are fields containing complex types deriving from IReflectable. They
	 * 			are serialized recursively and you may add/remove fields from them without breaking
	 * 			the serialized data.
	 * 			
	 *			ReflectablePtr field are different from Reflectable fields because other types may reference
	 *			the same Reflectable object using a ReflectablePtr, while normal Reflectable fields are only
	 *			referenced by a single field they're declared on.
	 */
	struct RTTIReflectablePtrFieldBase : public RTTIField
	{
		/**
		 * @brief	Retrieves the IReflectable value from the provided instance.
		 * 			
		 * @note	Field type must not be an array.
		 */
		virtual std::shared_ptr<IReflectable> getValue(void* object) = 0;

		/**
		 * @brief	Retrieves the IReflectable value from an array on the provided instance
		 * 			and index.
		 * 			
		 * @note	Field type must be an array.
		 */
		virtual std::shared_ptr<IReflectable> getArrayValue(void* object, UINT32 index) = 0;

		/**
		 * @brief	Sets the IReflectable value in the provided instance.
		 * 			
		 * @note	Field type must not be an array.
		 */
		virtual void setValue(void* object, std::shared_ptr<IReflectable> value) = 0;

		/**
		 * @brief	Sets the IReflectable value in an array on the provided instance
		 * 			and index.
		 * 			
		 * @note	Field type must be an array.
		 */
		virtual void setArrayValue(void* object, UINT32 index, std::shared_ptr<IReflectable> value) = 0;

		/**
		 * @brief	Creates a new object of the field type.
		 */
		virtual std::shared_ptr<IReflectable> newObject() = 0;

		/**
		 * @brief	Returns the RTTI identifier of the class owning the field.
		 */
		virtual UINT32 getRTTIId() = 0;

		/**
		 * @brief	Returns the name of the class owning the field.
		 */
		virtual const String& getRTTIName() = 0;

		/**
		 * @copydoc RTTIField::hasDynamicSize
		 */
		virtual bool hasDynamicSize() { return true; }
	};

	/**
	 * @brief	Class containing a reflectable pointer field containing a specific type.
	 */
	template <class DataType, class ObjectType>
	struct RTTIReflectablePtrField : public RTTIReflectablePtrFieldBase
	{
		/**
		 * @brief	Initializes a field pointing to a single data type implementing IReflectable interface.
		 *
		 * @param	name		Name of the field.
		 * @param	uniqueId	Unique identifier for this field. Although name is also a unique
		 * 						identifier we want a small data type that can be used for efficiently
		 * 						serializing data to disk and similar. It is primarily used for compatibility
		 * 						between different versions of serialized data.
		 * @param	getter  	The getter method for the field. Must be a specific signature: DataType*(ObjectType*)
		 * @param	setter  	The setter method for the field. Must be a specific signature: void(ObjectType*, DataType*)
		 * @param	flags		Various flags you can use to specialize how systems handle this field. See "RTTIFieldFlag".
		 */
		void initSingle(const String& name, UINT16 uniqueId, Any getter, Any setter, UINT64 flags)
		{
			initAll(getter, setter, nullptr, nullptr, name, uniqueId, false, SerializableFT_ReflectablePtr, flags);
		}

		/**
		 * @brief	Initializes a field containing an array of pointers to data types implementing IReflectable interface.
		 *
		 * @param	name		Name of the field.
		 * @param	uniqueId	Unique identifier for this field. Although name is also a unique
		 * 						identifier we want a small data type that can be used for efficiently
		 * 						serializing data to disk and similar. It is primarily used for compatibility
		 * 						between different versions of serialized data.
		 * @param	getter  	The getter method for the field. Must be a specific signature: DataType*(ObjectType*, UINT32)
		 * @param	getSize 	Getter method that returns the size of an array. Must be a specific signature: UINT32(ObjectType*)
		 * @param	setter  	The setter method for the field. Must be a specific signature: void(ObjectType*, UINT32, DataType*)
		 * @param	setSize 	Setter method that allows you to resize an array. Can be null. Must be a specific signature: void(ObjectType*, UINT32)
		 * @param	flags		Various flags you can use to specialize how systems handle this field. See "RTTIFieldFlag".
		 */
		void initArray(const String& name, UINT16 uniqueId, Any getter,
			Any getSize, Any setter, Any setSize, UINT64 flags)
		{
			initAll(getter, setter, getSize, setSize, name, uniqueId, true, SerializableFT_ReflectablePtr, flags);
		}

		/**
		 * @copydoc RTTIField::getTypeSize
		 */
		virtual UINT32 getTypeSize()
		{
			return 0; // Complex types don't store size the conventional way
		}

		/**
		 * @copydoc RTTIReflectablePtrFieldBase::getValue
		 */
		virtual std::shared_ptr<IReflectable> getValue(void* object)
		{
			checkIsArray(false);

			ObjectType* castObjType = static_cast<ObjectType*>(object);
			std::function<std::shared_ptr<DataType>(ObjectType*)> f = any_cast<std::function<std::shared_ptr<DataType>(ObjectType*)>>(valueGetter);
			std::shared_ptr<IReflectable> castDataType = f(castObjType);

			return castDataType;
		}

		/**
		 * @copydoc RTTIReflectablePtrFieldBase::getArrayValue
		 */
		virtual std::shared_ptr<IReflectable> getArrayValue(void* object, UINT32 index)
		{
			checkIsArray(true);

			ObjectType* castObjType = static_cast<ObjectType*>(object);
			std::function<std::shared_ptr<DataType>(ObjectType*, UINT32)> f = any_cast<std::function<std::shared_ptr<DataType>(ObjectType*, UINT32)>>(valueGetter);

			std::shared_ptr<IReflectable> castDataType = f(castObjType, index);
			return castDataType;
		}

		/**
		 * @copydoc RTTIReflectablePtrFieldBase::setValue
		 */
		virtual void setValue(void* object, std::shared_ptr<IReflectable> value)
		{
			checkIsArray(false);

			if(valueSetter.empty())
			{
				BS_EXCEPT(InternalErrorException,
					"Specified field (" + mName + ") has no setter.");
			}

			ObjectType* castObjType = static_cast<ObjectType*>(object);
			std::shared_ptr<DataType> castDataObj = std::static_pointer_cast<DataType>(value);
			std::function<void(ObjectType*, std::shared_ptr<DataType>)> f = any_cast<std::function<void(ObjectType*, std::shared_ptr<DataType>)>>(valueSetter);
			f(castObjType, castDataObj);
		}

		/**
		 * @copydoc RTTIReflectablePtrFieldBase::setArrayValue
		 */
		virtual void setArrayValue(void* object, UINT32 index, std::shared_ptr<IReflectable> value)
		{
			checkIsArray(true);

			if(valueSetter.empty())
			{
				BS_EXCEPT(InternalErrorException, 
					"Specified field (" + mName + ") has no setter.");
			}

			ObjectType* castObjType = static_cast<ObjectType*>(object);
			std::shared_ptr<DataType> castDataObj = std::static_pointer_cast<DataType>(value);
			std::function<void(ObjectType*, UINT32, std::shared_ptr<DataType>)> f = any_cast<std::function<void(ObjectType*, UINT32, std::shared_ptr<DataType>)>>(valueSetter);
			f(castObjType, index, castDataObj);
		}

		/**
		 * @copydoc RTTIField::setArraySize
		 */
		virtual UINT32 getArraySize(void* object)
		{
			checkIsArray(true);

			std::function<UINT32(ObjectType*)> f = any_cast<std::function<UINT32(ObjectType*)>>(arraySizeGetter);
			ObjectType* castObject = static_cast<ObjectType*>(object);
			return f(castObject);
		}

		/**
		 * @copydoc RTTIField::setArraySize
		 */
		virtual void setArraySize(void* object, UINT32 size)
		{
			checkIsArray(true);

			if(arraySizeSetter.empty())
			{
				BS_EXCEPT(InternalErrorException, 
					"Specified field (" + mName + ") has no array size setter.");
			}

			std::function<void(ObjectType*, UINT32)> f = any_cast<std::function<void(ObjectType*, UINT32)>>(arraySizeSetter);
			ObjectType* castObject = static_cast<ObjectType*>(object);
			f(castObject, size);
		}

		/**
		 * @copydoc RTTIReflectablePtrFieldBase::newObject
		 */
		virtual std::shared_ptr<IReflectable> newObject()
		{
			return std::shared_ptr<IReflectable>(DataType::getRTTIStatic()->newRTTIObject());
		}

		/**
		 * @copydoc RTTIReflectablePtrFieldBase::getRTTIId
		 */
		virtual UINT32 getRTTIId()
		{
			return DataType::getRTTIStatic()->getRTTIId();
		}

		/**
		 * @copydoc RTTIReflectablePtrFieldBase::getRTTIName
		 */
		virtual const String& getRTTIName()
		{
			return DataType::getRTTIStatic()->getRTTIName();
		}
	};
}
#pragma once

#include "BsPrerequisitesUtil.h"
#include "BsRTTIField.h"
#include "BsIReflectable.h"

namespace BansheeEngine
{
	/**
	 * @brief	Base class containing common functionality for a reflectable class field. 
	 * 			
	 * @note	Reflectable fields are fields containing complex types deriving from IReflectable. They
	 * 			are serialized recursively and you may add/remove fields from them without breaking
	 * 			the serialized data.
	 */
	struct RTTIReflectableFieldBase : public RTTIField
	{
		/**
		 * @brief	Retrieves the IReflectable value from the provided instance.
		 * 			
		 * @note	Field type must not be an array.
		 */
		virtual IReflectable& getValue(void* object) = 0;

		/**
		 * @brief	Retrieves the IReflectable value from an array on the provided instance
		 * 			and index.
		 * 			
		 * @note	Field type must be an array.
		 */
		virtual IReflectable& getArrayValue(void* object, UINT32 index) = 0;

		/**
		 * @brief	Sets the IReflectable value in the provided instance.
		 * 			
		 * @note	Field type must not be an array.
		 */
		virtual void setValue(void* object, IReflectable& value) = 0;

		/**
		 * @brief	Sets the IReflectable value in an array on the provided instance
		 * 			and index.
		 * 			
		 * @note	Field type must be an array.
		 */
		virtual void setArrayValue(void* object, UINT32 index, IReflectable& value) = 0;

		/**
		 * @brief	Creates a new object of the field type.
		 */
		virtual std::shared_ptr<IReflectable> newObject() = 0;

		/**
		 * @copydoc RTTIField::hasDynamicSize
		 */
		virtual bool hasDynamicSize() { return true; }

		/**
		 * @brief	Retrieves the RTTI object for the type the field contains.
		 */
		virtual RTTITypeBase* getType() = 0;
	};

	/**
	 * @brief	Class containing a reflectable field containing a specific type.
	 */
	template <class DataType, class ObjectType>
	struct RTTIReflectableField : public RTTIReflectableFieldBase
	{
		/**
		 * @brief	Initializes a field containing a single data type implementing IReflectable interface. 
		 *
		 * @param	name		Name of the field.
		 * @param	uniqueId	Unique identifier for this field. Although name is also a unique
		 * 						identifier we want a small data type that can be used for efficiently
		 * 						serializing data to disk and similar. It is primarily used for compatibility
		 * 						between different versions of serialized data.
		 * @param	getter  	The getter method for the field. Must be a specific signature: DataType&(ObjectType*)
		 * @param	setter  	The setter method for the field. Must be a specific signature: void(ObjectType*, DataType)
		 * @param	flags		Various flags you can use to specialize how systems handle this field. See "RTTIFieldFlag".
		 */
		void initSingle(const String& name, UINT16 uniqueId, Any getter, Any setter, UINT64 flags)
		{
			initAll(getter, setter, nullptr, nullptr, name, uniqueId, false, SerializableFT_Reflectable, flags);
		}

		/**
		 * @brief	Initializes a field containing an array of data types implementing IReflectable interface.
		 *
		 * @param	name		Name of the field.
		 * @param	uniqueId	Unique identifier for this field. Although name is also a unique
		 * 						identifier we want a small data type that can be used for efficiently
		 * 						serializing data to disk and similar. It is primarily used for compatibility
		 * 						between different versions of serialized data.
		 * @param	getter  	The getter method for the field. Must be a specific signature: DataType&(ObjectType*, UINT32)
		 * @param	getSize 	Getter method that returns the size of an array. Must be a specific signature: UINT32(ObjectType*)
		 * @param	setter  	The setter method for the field. Must be a specific signature: void(ObjectType*, UINT32, DataType)
		 * @param	setSize 	Setter method that allows you to resize an array. Must be a specific signature: void(ObjectType*, UINT32)
		 * @param	flags		Various flags you can use to specialize how systems handle this field. See "RTTIFieldFlag".
		 */
		void initArray(const String& name, UINT16 uniqueId, Any getter,
			Any getSize, Any setter, Any setSize, UINT64 flags)
		{
			initAll(getter, setter, getSize, setSize, name, uniqueId, true, SerializableFT_Reflectable, flags);
		}

		/**
		 * @copydoc RTTIField::getTypeSize
		 */
		virtual UINT32 getTypeSize()
		{
			return 0; // Complex types don't store size the conventional way
		}

		/**
		 * @copydoc RTTIReflectableFieldBase::getValue
		 */
		virtual IReflectable& getValue(void* object)
		{
			checkIsArray(false);

			ObjectType* castObjType = static_cast<ObjectType*>(object);
			std::function<DataType&(ObjectType*)> f = any_cast<std::function<DataType&(ObjectType*)>>(valueGetter);
			IReflectable& castDataType = f(castObjType);

			return castDataType;
		}

		/**
		 * @copydoc RTTIReflectableFieldBase::getArrayValue
		 */
		virtual IReflectable& getArrayValue(void* object, UINT32 index)
		{
			checkIsArray(true);

			ObjectType* castObjType = static_cast<ObjectType*>(object);
			std::function<DataType&(ObjectType*, UINT32)> f = any_cast<std::function<DataType&(ObjectType*, UINT32)>>(valueGetter);

			IReflectable& castDataType = f(castObjType, index);
			return castDataType;
		}

		/**
		 * @copydoc RTTIReflectableFieldBase::setValue
		 */
		virtual void setValue(void* object, IReflectable& value)
		{
			checkIsArray(false);

			if(valueSetter.empty())
			{
				BS_EXCEPT(InternalErrorException,
					"Specified field (" + mName + ") has no setter.");
			}

			ObjectType* castObjType = static_cast<ObjectType*>(object);
			DataType& castDataObj = static_cast<DataType&>(value);
			std::function<void(ObjectType*, DataType&)> f = any_cast<std::function<void(ObjectType*, DataType&)>>(valueSetter);
			f(castObjType, castDataObj);
		}

		/**
		 * @copydoc RTTIReflectableFieldBase::setArrayValue
		 */
		virtual void setArrayValue(void* object, UINT32 index, IReflectable& value)
		{
			checkIsArray(true);

			if(valueSetter.empty())
			{
				BS_EXCEPT(InternalErrorException, 
					"Specified field (" + mName + ") has no setter.");
			}

			ObjectType* castObjType = static_cast<ObjectType*>(object);
			DataType& castDataObj = static_cast<DataType&>(value);
			std::function<void(ObjectType*, UINT32, DataType&)> f = any_cast<std::function<void(ObjectType*, UINT32, DataType&)>>(valueSetter);
			f(castObjType, index, castDataObj);
		}

		/**
		 * @copydoc RTTIField::getArraySize
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
		 * @copydoc RTTIReflectableFieldBase::newObject
		 */
		virtual std::shared_ptr<IReflectable> newObject()
		{
			return DataType::getRTTIStatic()->newRTTIObject();
		}

		/**
		 * @copydoc RTTIReflectableFieldBase::getType
		 */
		virtual RTTITypeBase* getType()
		{
			return DataType::getRTTIStatic();
		}
	};
}
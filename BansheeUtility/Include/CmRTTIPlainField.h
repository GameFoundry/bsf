#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmRTTIField.h"
#include "CmException.h"

namespace BansheeEngine
{
	/**
	 * @brief	Base class containing common functionality for a plain class field. 
	 * 			
	 * @note	Plain fields are considered those that may be serialized directly by copying their memory.
	 * 			(All built-in types, strings, etc.)
	 */
	struct RTTIPlainFieldBase : public RTTIField
	{
		/**
		 * @brief	Throws an exception if the current field type and provided template types don't match.
		 */
		template<class DataType>
		void checkType()
		{
			// TODO: Low priority. Because I wanted to get rid of SerializableType I have no way of checking the actual
			// type of the field and the type provided to get/set methods matches

			/*if(mType.id != SerializableType<DataType>().id)
			{
				BS_EXCEPT(InternalErrorException,
					"Invalid field type.",
					"SerializableSimpleTypeFieldBase::checkType()");
			}*/
		}
		
		/**
		 * @copydoc RTTIField::getTypeId
		 */
		virtual UINT32 getTypeId()
		{
			return 0;
		}

		/**
		 * @copydoc RTTIField::hasDynamicSize
		 */
		virtual bool hasDynamicSize()
		{
			return false;
		}

		/**
		 * @brief	Gets the dynamic size of the object. If object has no dynamic size,
		 * 			static size of the object is returned.
		 */
		virtual UINT32 getDynamicSize(void* object)
		{
			return 0;
		}

		/**
		 * @brief	Gets the dynamic size of an array element. If the element has no dynamic size,
		 * 			static size of the element is returned.
		 */
		virtual UINT32 getArrayElemDynamicSize(void* object, int index)
		{
			return 0;
		}

		/**
		 * @brief	Retrieves the value from the provided field of the provided object, and copies
		 * 			it into the buffer. WARNING - It does not check if buffer is large enough.
		 */
		virtual void toBuffer(void* object, void* buffer) = 0;

		/**
		 * @brief	Retrieves the value at the specified array index on the provided field of the
		 * 			provided object, and copies it into the buffer. WARNING - It does not check if buffer
		 * 			is large enough.
		 */
		virtual void arrayElemToBuffer(void* object, int index, void* buffer) = 0;

		/**
		 * @brief	Sets the value on the provided field of the provided object. Value is copied from the buffer. 
		 * 			WARNING - It does not check the value in the buffer in any way. You must make sure buffer points
		 * 			to the proper location and contains the proper type.
		 */
		virtual void fromBuffer(void* object, void* buffer) = 0;

		/**
		 * @brief	Sets the value at the specified array index on the provided field of the provided
		 * 			object. Value is copied from the buffer. WARNING - It does not check the value in the
		 * 			buffer in any way. You must make sure buffer points to the proper location and
		 * 			contains the proper type.
		 */
		virtual void arrayElemFromBuffer(void* object, int index, void* buffer) = 0;
	};

	/**
	 * @brief	Represents a plain class field containing a specific type.
	 */
	template <class DataType, class ObjectType>
	struct RTTIPlainField : public RTTIPlainFieldBase
	{
		/**
		 * @brief	Initializes a plain field containing a single value.
		 *
		 * @param	name		Name of the field.
		 * @param	uniqueId	Unique identifier for this field. Although name is also a unique
		 * 						identifier we want a small data type that can be used for efficiently
		 * 						serializing data to disk and similar. It is primarily used for compatibility
		 * 						between different versions of serialized data.
		 * @param	getter  	The getter method for the field. Must be a specific signature: DataType(ObjectType*).
		 * @param	setter  	The setter method for the field. Must be a specific signature: void(ObjectType*, DataType)
		 * @param	flags		Various flags you can use to specialize how outside systems handle this field. See "RTTIFieldFlag".
		 */
		void initSingle(const String& name, UINT16 uniqueId, Any getter, Any setter, UINT64 flags)
		{
			int typeId = RTTIPlainType<DataType>::id; // Just making sure provided type has a type ID

			static_assert((RTTIPlainType<DataType>::hasDynamicSize != 0 || (sizeof(DataType) <= 255)), 
				"Trying to create a plain RTTI field with size larger than 255. In order to use larger sizes for plain types please specialize " \
				" RTTIPlainType, set hasDynamicSize to true.");

			initAll(getter, setter, nullptr, nullptr, name, uniqueId, false, SerializableFT_Plain, flags);
		}

		/**
		 * @brief	Initializes a plain field containing multiple values in an array. 
		 *
		 * @param	name		Name of the field.
		 * @param	uniqueId	Unique identifier for this field. Although name is also a unique
		 * 						identifier we want a small data type that can be used for efficiently
		 * 						serializing data to disk and similar. It is primarily used for compatibility
		 * 						between different versions of serialized data.
		 * @param	getter  	The getter method for the field. Must be a specific signature: DataType(ObjectType*, UINT32)
		 * @param	getSize 	Getter method that returns the size of an array. Must be a specific signature: UINT32(ObjectType*)
		 * @param	setter  	The setter method for the field. Must be a specific signature: void(ObjectType*, UINT32, DataType)
		 * @param	setSize 	Setter method that allows you to resize an array. Can be null. Must be a specific signature: void(ObjectType*, UINT32)
		 * @param	flags		Various flags you can use to specialize how outside systems handle this field. See "RTTIFieldFlag".
		 */
		void initArray(const String& name, UINT16 uniqueId, Any getter,
			Any getSize, Any setter, Any setSize, UINT64 flags)
		{
			int typeId = RTTIPlainType<DataType>::id; // Just making sure provided type has a type ID

			static_assert((RTTIPlainType<DataType>::hasDynamicSize != 0 || (sizeof(DataType) <= 255)), 
				"Trying to create a plain RTTI field with size larger than 255. In order to use larger sizes for plain types please specialize " \
				" RTTIPlainType, set hasDynamicSize to true.");

			initAll(getter, setter, getSize, setSize, name, uniqueId, true, SerializableFT_Plain, flags);
		}

		/**
		 * @copydoc RTTIField::getTypeSize
		 */
		virtual UINT32 getTypeSize()
		{
			return sizeof(DataType);
		}

		/**
		 * @copydoc RTTIPlainFieldBase::getTypeId
		 */
		virtual UINT32 getTypeId()
		{
			return RTTIPlainType<DataType>::id;
		}

		/**
		 * @copydoc RTTIPlainFieldBase::hasDynamicSize
		 */
		virtual bool hasDynamicSize()
		{
			return RTTIPlainType<DataType>::hasDynamicSize != 0;
		}

		/**
		 * @copydoc RTTIPlainFieldBase::getDynamicSize
		 */
		virtual UINT32 getDynamicSize(void* object)
		{
			checkIsArray(false);
			checkType<DataType>();

			ObjectType* castObject = static_cast<ObjectType*>(object);

			std::function<DataType&(ObjectType*)> f = any_cast<std::function<DataType&(ObjectType*)>>(valueGetter);
			DataType value = f(castObject);

			return RTTIPlainType<DataType>::getDynamicSize(value);
		}

		/**
		 * @copydoc RTTIPlainFieldBase::getArrayElemDynamicSize
		 */
		virtual UINT32 getArrayElemDynamicSize(void* object, int index)
		{
			checkIsArray(true);
			checkType<DataType>();

			ObjectType* castObject = static_cast<ObjectType*>(object);

			std::function<DataType&(ObjectType*, UINT32)> f = any_cast<std::function<DataType&(ObjectType*, UINT32)>>(valueGetter);
			DataType value = f(castObject, index);

			return RTTIPlainType<DataType>::getDynamicSize(value);
		}

		/**
		 * @copydoc RTTIPlainField::getArraySize
		 */
		virtual UINT32 getArraySize(void* object)
		{
			checkIsArray(true);

			std::function<UINT32(ObjectType*)> f = any_cast<std::function<UINT32(ObjectType*)>>(arraySizeGetter);
			ObjectType* castObject = static_cast<ObjectType*>(object);
			return f(castObject);
		}

		/**
		 * @copydoc RTTIPlainField::setArraySize
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
		 * @copydoc RTTIPlainFieldBase::toBuffer
		 */
		virtual void toBuffer(void* object, void* buffer)
		{
			checkIsArray(false);
			checkType<DataType>();

			ObjectType* castObject = static_cast<ObjectType*>(object);

			std::function<DataType&(ObjectType*)> f = any_cast<std::function<DataType&(ObjectType*)>>(valueGetter);
			DataType value = f(castObject);

			RTTIPlainType<DataType>::toMemory(value, (char*)buffer);
		}

		/**
		 * @copydoc RTTIPlainFieldBase::arrayElemToBuffer
		 */
		virtual void arrayElemToBuffer(void* object, int index, void* buffer)
		{
			checkIsArray(true);
			checkType<DataType>();

			ObjectType* castObject = static_cast<ObjectType*>(object);

			std::function<DataType&(ObjectType*, UINT32)> f = any_cast<std::function<DataType&(ObjectType*, UINT32)>>(valueGetter);
			DataType value = f(castObject, index);

			RTTIPlainType<DataType>::toMemory(value, (char*)buffer);
		}

		/**
		 * @copydoc RTTIPlainFieldBase::fromBuffer
		 */
		virtual void fromBuffer(void* object, void* buffer)
		{
			checkIsArray(false);
			checkType<DataType>();

			ObjectType* castObject = static_cast<ObjectType*>(object);

			DataType value;
			RTTIPlainType<DataType>::fromMemory(value, (char*)buffer);

			if(valueSetter.empty())
			{
				BS_EXCEPT(InternalErrorException,
					"Specified field (" + mName + ") has no setter.");
			}

			std::function<void(ObjectType*, DataType&)> f = any_cast<std::function<void(ObjectType*, DataType&)>>(valueSetter);
			f(castObject, value);
		}

		/**
		 * @copydoc RTTIPlainFieldBase::arrayElemFromBuffer
		 */
		virtual void arrayElemFromBuffer(void* object, int index, void* buffer)
		{
			checkIsArray(true);
			checkType<DataType>();

			ObjectType* castObject = static_cast<ObjectType*>(object);

			DataType value;
			RTTIPlainType<DataType>::fromMemory(value, (char*)buffer);

			if(valueSetter.empty())
			{
				BS_EXCEPT(InternalErrorException, 
					"Specified field (" + mName + ") has no setter.");
			}

			std::function<void(ObjectType*, UINT32, DataType&)> f = any_cast<std::function<void(ObjectType*, UINT32, DataType&)>>(valueSetter);
			f(castObject, index, value);
		}
	};
}
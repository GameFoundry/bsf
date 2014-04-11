#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmRTTIField.h"
#include "CmException.h"

namespace CamelotFramework
{
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
				CM_EXCEPT(InternalErrorException,
					"Invalid field type.",
					"SerializableSimpleTypeFieldBase::checkType()");
			}*/
		}

		virtual UINT32 getTypeId()
		{
			return 0;
		}

		virtual bool hasDynamicSize()
		{
			return false;
		}

		virtual UINT32 getDynamicSize(void* object)
		{
			return 0;
		}

		virtual UINT32 getArrayElemDynamicSize(void* object, int index)
		{
			return 0;
		}

		/**
		 * @brief	Retrieves the value from the provided field of the provided object, and copies
		 * 			it into the buffer. WARNING - It does not check if buffer is large enough.
		 * 			
		 * @note	Size of the data copied to buffer is mType.size
		 */
		virtual void toBuffer(void* object, void* buffer) = 0;

		/**
		 * @brief	Retrieves the value at the specified array index on the provided field of the
		 * 			provided object, and copies it into the buffer. WARNING - It does not check if buffer
		 * 			is large enough.
		 * 			
		 * @note	Size of the data copied to buffer is mType.size
		 */
		virtual void arrayElemToBuffer(void* object, int index, void* buffer) = 0;

		/**
		 * @brief	Sets the value on the provided field of the provided object. Value is copied from the buffer. 
		 * 			WARNING - It does not check the value in the buffer in any way. You must make sure buffer points
		 * 			to the proper location and contains the proper type.
		 * 			
		 * @note	Size of the data copied from buffer is mType.size
		 */
		virtual void fromBuffer(void* object, void* buffer) = 0;

		/**
		 * @brief	Sets the value at the specified array index on the provided field of the provided
		 * 			object. Value is copied from the buffer. WARNING - It does not check the value in the
		 * 			buffer in any way. You must make sure buffer points to the proper location and
		 * 			contains the proper type.
		 * 			
		 * 	@note	Size of the data copied from buffer is mType.size.
		 */
		virtual void arrayElemFromBuffer(void* object, int index, void* buffer) = 0;
	};

	template <class DataType, class ObjectType>
	struct RTTIPlainField : public RTTIPlainFieldBase
	{
		/**
		 * @brief	Initializes a field with one of the built-in types. You may provide your own type ID,
		 * 			just make sure it doesn't conflict with any standard types.
		 *
		 * @param	name		Name of the field.
		 * @param	uniqueId	Unique identifier for this field. Although name is also a unique
		 * 						identifier we want a small data type that can be used for efficiently
		 * 						serializing data to disk and similar. It is primarily used for compatibility
		 * 						between different versions of serialized data.
		 * @param	getter  	The getter method for the field. Cannot be null. Must be a specific signature: DataType(ObjectType*).
		 * @param	setter  	The setter method for the field. Can be null. Must be a specific signature: void(ObjectType*, DataType)
		 * @param	flags		Various flags you can use to specialize how systems handle this field
		 */
		void initSingle(const String& name, UINT16 uniqueId, boost::any getter, boost::any setter, UINT64 flags)
		{
			int typeId = RTTIPlainType<DataType>::id; // Just making sure provided type has a type ID

			BOOST_STATIC_ASSERT_MSG((RTTIPlainType<DataType>::hasDynamicSize != 0 || (sizeof(DataType) <= 255)), 
				"Trying to create a plain RTTI field with size larger than 255. In order to use larger sizes for plain types please specialize " \
				" RTTIPlainType, set hasDynamicSize to true.");

			initAll(getter, setter, nullptr, nullptr, name, uniqueId, false, SerializableFT_Plain, flags);
		}

		/**
		 * @brief	Initializes a VECTOR field with one of the built-in types. You may provide your own
		 * 			type ID, just make sure it doesn't conflict with any standard types.
		 *
		 * @param	name		Name of the field.
		 * @param	uniqueId	Unique identifier for this field. Although name is also a unique
		 * 						identifier we want a small data type that can be used for efficiently
		 * 						serializing data to disk and similar. It is primarily used for compatibility
		 * 						between different versions of serialized data.
		 * @param	getter  	The getter method for the field. Cannot be null. Must be a specific signature: DataType(ObjectType*, UINT32)
		 * @param	getSize 	Getter method that returns the size of an array. Cannot be null. Must be a specific signature: UINT32(ObjectType*)
		 * @param	setter  	The setter method for the field. Can be null. Must be a specific signature: void(ObjectType*, UINT32, DataType)
		 * @param	setSize 	Setter method that allows you to resize an array. Can be null. Must be a specific signature: void(ObjectType*, UINT32)
		 * @param	flags		Various flags you can use to specialize how systems handle this field
		 */
		void initArray(const String& name, UINT16 uniqueId, boost::any getter, 
			boost::any getSize, boost::any setter, boost::any setSize, UINT64 flags)
		{
			int typeId = RTTIPlainType<DataType>::id; // Just making sure provided type has a type ID

			BOOST_STATIC_ASSERT_MSG((RTTIPlainType<DataType>::hasDynamicSize != 0 || (sizeof(DataType) <= 255)), 
				"Trying to create a plain RTTI field with size larger than 255. In order to use larger sizes for plain types please specialize " \
				" RTTIPlainType, set hasDynamicSize to true.");

			initAll(getter, setter, getSize, setSize, name, uniqueId, true, SerializableFT_Plain, flags);
		}

		virtual UINT32 getTypeSize()
		{
			return sizeof(DataType);
		}

		virtual UINT32 getTypeId()
		{
			return RTTIPlainType<DataType>::id;
		}

		virtual bool hasDynamicSize()
		{
			return RTTIPlainType<DataType>::hasDynamicSize != 0;
		}

		virtual UINT32 getDynamicSize(void* object)
		{
			checkIsArray(false);
			checkType<DataType>();

			ObjectType* castObject = static_cast<ObjectType*>(object);

			std::function<DataType&(ObjectType*)> f = boost::any_cast<std::function<DataType&(ObjectType*)>>(valueGetter);
			DataType value = f(castObject);

			return RTTIPlainType<DataType>::getDynamicSize(value);
		}

		virtual UINT32 getArrayElemDynamicSize(void* object, int index)
		{
			checkIsArray(true);
			checkType<DataType>();

			ObjectType* castObject = static_cast<ObjectType*>(object);

			std::function<DataType&(ObjectType*, UINT32)> f = boost::any_cast<std::function<DataType&(ObjectType*, UINT32)>>(valueGetter);
			DataType value = f(castObject, index);

			return RTTIPlainType<DataType>::getDynamicSize(value);
		}

		virtual UINT32 getArraySize(void* object)
		{
			checkIsArray(true);

			std::function<UINT32(ObjectType*)> f = boost::any_cast<std::function<UINT32(ObjectType*)>>(arraySizeGetter);
			ObjectType* castObject = static_cast<ObjectType*>(object);
			return f(castObject);
		}

		virtual void setArraySize(void* object, UINT32 size)
		{
			checkIsArray(true);

			if(arraySizeSetter.empty())
			{
				CM_EXCEPT(InternalErrorException, 
					"Specified field (" + mName + ") has no array size setter.");
			}

			std::function<void(ObjectType*, UINT32)> f = boost::any_cast<std::function<void(ObjectType*, UINT32)>>(arraySizeSetter);
			ObjectType* castObject = static_cast<ObjectType*>(object);
			f(castObject, size);
		}

		virtual void toBuffer(void* object, void* buffer)
		{
			checkIsArray(false);
			checkType<DataType>();

			ObjectType* castObject = static_cast<ObjectType*>(object);

			std::function<DataType&(ObjectType*)> f = boost::any_cast<std::function<DataType&(ObjectType*)>>(valueGetter);
			DataType value = f(castObject);

			RTTIPlainType<DataType>::toMemory(value, (char*)buffer);
		}

		virtual void arrayElemToBuffer(void* object, int index, void* buffer)
		{
			checkIsArray(true);
			checkType<DataType>();

			ObjectType* castObject = static_cast<ObjectType*>(object);

			std::function<DataType&(ObjectType*, UINT32)> f = boost::any_cast<std::function<DataType&(ObjectType*, UINT32)>>(valueGetter);
			DataType value = f(castObject, index);

			RTTIPlainType<DataType>::toMemory(value, (char*)buffer);
		}

		virtual void fromBuffer(void* object, void* buffer)
		{
			checkIsArray(false);
			checkType<DataType>();

			ObjectType* castObject = static_cast<ObjectType*>(object);

			DataType value;
			RTTIPlainType<DataType>::fromMemory(value, (char*)buffer);

			if(valueSetter.empty())
			{
				CM_EXCEPT(InternalErrorException,
					"Specified field (" + mName + ") has no setter.");
			}

			std::function<void(ObjectType*, DataType&)> f = boost::any_cast<std::function<void(ObjectType*, DataType&)>>(valueSetter);
			f(castObject, value);
		}

		virtual void arrayElemFromBuffer(void* object, int index, void* buffer)
		{
			checkIsArray(true);
			checkType<DataType>();

			ObjectType* castObject = static_cast<ObjectType*>(object);

			DataType value;
			RTTIPlainType<DataType>::fromMemory(value, (char*)buffer);

			if(valueSetter.empty())
			{
				CM_EXCEPT(InternalErrorException, 
					"Specified field (" + mName + ") has no setter.");
			}

			std::function<void(ObjectType*, UINT32, DataType&)> f = boost::any_cast<std::function<void(ObjectType*, UINT32, DataType&)>>(valueSetter);
			f(castObject, index, value);
		}
	};
}
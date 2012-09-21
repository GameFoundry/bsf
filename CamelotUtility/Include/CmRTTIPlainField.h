#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmRTTIField.h"

namespace CamelotEngine
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

		template<class ObjectType, class DataType>
		void getValue(ObjectType* object, DataType& value)
		{
			checkIsArray(false);
			checkType<DataType>();

			boost::function<DataType(ObjectType*)> f = boost::any_cast<boost::function<DataType(ObjectType*)>>(valueGetter);
			value = f(object);
		}

		template<class ObjectType, class DataType>
		void getArrayValue(ObjectType* object, UINT32 index, DataType& value)
		{
			checkIsArray(true);
			checkType<DataType>();

			boost::function<DataType(ObjectType*, UINT32)> f = boost::any_cast<boost::function<DataType(ObjectType*, UINT32)>>(valueGetter);
			value = f(object, index);
		}

		template<class ObjectType, class DataType>
		void setValue(ObjectType* object, DataType& value)
		{
			checkIsArray(false);
			checkType<DataType>();

			if(valueSetter.empty())
			{
				CM_EXCEPT(InternalErrorException,
					"Specified field (" + mName + ") has no setter.");
			}

			boost::function<void(ObjectType*, DataType)> f = boost::any_cast<boost::function<void(ObjectType*, DataType)>>(valueSetter);
			f(object, value);
		}

		template<class ObjectType, class DataType>
		void setArrayValue(ObjectType* object, UINT32 index, DataType& value)
		{
			checkIsArray(true);
			checkType<DataType>();

			if(valueSetter.empty())
			{
				CM_EXCEPT(InternalErrorException, 
					"Specified field (" + mName + ") has no setter.");
			}

			boost::function<void(ObjectType*, UINT32, DataType)> f = boost::any_cast<boost::function<void(ObjectType*, UINT32, DataType)>>(valueSetter);
			f(object, index, value);
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
		 * 						
		 * @note	IMPORTANT - Type returned/set by getter/setter must provide "public static int TYPE_ID" field, which can be used for identifying the
		 * 			type. You can set the ID to anything you wish, as it is not used internally. However its possible that other engine modules do require
		 * 			you to set this field to a unique value. Native types do not need (or can) set this field. 
		 */
		void initSingle(const std::string& name, UINT16 uniqueId, boost::any getter, boost::any setter)
		{
			int typeId = SerializableSimpleType<DataType>::id; // Just making sure provided type has a type ID

			initAll(getter, setter, nullptr, nullptr, name, uniqueId, false, SerializableFT_Plain);
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
		 * 						
		 * @note	IMPORTANT - Type returned/set by getter/setter must provide "public static UINT32 TYPE_ID" field, which can be used for identifying the
		 * 			type. You can set the ID to anything you wish, as it is not used internally. However its possible that other engine modules do require
		 * 			you to set this field to a unique value. Native types do not need (or can) set this field.
		 */
		void initArray(const std::string& name, UINT16 uniqueId, boost::any getter, 
			boost::any getSize, boost::any setter, boost::any setSize)
		{
			int typeId = SerializableSimpleType<DataType>::id; // Just making sure provided type has a type ID

			initAll(getter, setter, getSize, setSize, name, uniqueId, true, SerializableFT_Plain);
		}

		virtual UINT32 getTypeSize()
		{
			return sizeof(DataType);
		}

		virtual UINT32 getTypeId()
		{
			return SerializableSimpleType<DataType>::id;
		}

		virtual UINT32 getArraySize(void* object)
		{
			checkIsArray(true);

			boost::function<UINT32(ObjectType*)> f = boost::any_cast<boost::function<UINT32(ObjectType*)>>(arraySizeGetter);
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

			boost::function<void(ObjectType*, UINT32)> f = boost::any_cast<boost::function<void(ObjectType*, UINT32)>>(arraySizeSetter);
			ObjectType* castObject = static_cast<ObjectType*>(object);
			f(castObject, size);
		}

		virtual void toBuffer(void* object, void* buffer)
		{
			checkIsArray(false);
			checkType<DataType>();

			ObjectType* castObject = static_cast<ObjectType*>(object);

			DataType value;
			getValue(castObject, value);
			memcpy(buffer, &value, getTypeSize());
		}

		virtual void arrayElemToBuffer(void* object, int index, void* buffer)
		{
			checkIsArray(true);
			checkType<DataType>();

			ObjectType* castObject = static_cast<ObjectType*>(object);

			DataType value;
			getArrayValue(castObject, index, value);
			memcpy(buffer, &value, getTypeSize());
		}

		virtual void fromBuffer(void* object, void* buffer)
		{
			checkIsArray(false);
			checkType<DataType>();

			ObjectType* castObject = static_cast<ObjectType*>(object);

			DataType value;
			memcpy(&value, buffer, getTypeSize());
			setValue(castObject, value);
		}

		virtual void arrayElemFromBuffer(void* object, int index, void* buffer)
		{
			checkIsArray(true);
			checkType<DataType>();

			ObjectType* castObject = static_cast<ObjectType*>(object);

			DataType value;
			memcpy(&value, buffer, getTypeSize());
			setArrayValue(castObject, index, value);
		}
	};
}
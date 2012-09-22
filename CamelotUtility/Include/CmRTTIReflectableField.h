#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmRTTIField.h"
#include "CmIReflectable.h"

namespace CamelotEngine
{
	struct RTTIReflectableFieldBase : public RTTIField
	{
		virtual IReflectable& getValue(void* object) = 0;
		virtual IReflectable& getArrayValue(void* object, UINT32 index) = 0;

		virtual void setValue(void* object, IReflectable& value) = 0;
		virtual void setArrayValue(void* object, UINT32 index, IReflectable& value) = 0;

		virtual IReflectable* newObject() = 0;

		virtual bool hasDynamicSize() { return true; }
	};

	template <class DataType, class ObjectType>
	struct RTTIReflectableField : public RTTIReflectableFieldBase
	{
		/**
		 * @brief	Initializes a field with a data type implementing IReflectable interface. 
		 *
		 * @param	name		Name of the field.
		 * @param	uniqueId	Unique identifier for this field. Although name is also a unique
		 * 						identifier we want a small data type that can be used for efficiently
		 * 						serializing data to disk and similar. It is primarily used for compatibility
		 * 						between different versions of serialized data.
		 * @param	getter  	The getter method for the field. Cannot be null. Must be a specific signature: DataType&(ObjectType*)
		 * @param	setter  	The setter method for the field. Can be null. Must be a specific signature: void(ObjectType*, DataType)
		 */
		void initSingle(const std::string& name, UINT16 uniqueId, boost::any getter, boost::any setter)
		{
			initAll(getter, setter, nullptr, nullptr, name, uniqueId, false, SerializableFT_Reflectable);
		}

		/**
		 * @brief	Initializes a VECTOR field with data type implementing by IReflectable interface.
		 *
		 * @param	name		Name of the field.
		 * @param	uniqueId	Unique identifier for this field. Although name is also a unique
		 * 						identifier we want a small data type that can be used for efficiently
		 * 						serializing data to disk and similar. It is primarily used for compatibility
		 * 						between different versions of serialized data.
		 * @param	getter  	The getter method for the field. Cannot be null. Must be a specific signature: DataType&(ObjectType*, UINT32)
		 * @param	getSize 	Getter method that returns the size of an array. Cannot be null. Must be a specific signature: UINT32(ObjectType*)
		 * @param	setter  	The setter method for the field. Can be null. Must be a specific signature: void(ObjectType*, UINT32, DataType)
		 * @param	setSize 	Setter method that allows you to resize an array. Can be null. Can be null. Must be a specific signature: void(ObjectType*, UINT32)
		 */
		void initArray(const std::string& name, UINT16 uniqueId, boost::any getter, 
			boost::any getSize, boost::any setter, boost::any setSize)
		{
			initAll(getter, setter, getSize, setSize, name, uniqueId, true, SerializableFT_Reflectable);
		}

		virtual UINT32 getTypeSize()
		{
			return 0; // Complex types don't store size the conventional way
		}

		virtual IReflectable& getValue(void* object)
		{
			checkIsArray(false);

			ObjectType* castObjType = static_cast<ObjectType*>(object);
			boost::function<DataType&(ObjectType*)> f = boost::any_cast<boost::function<DataType&(ObjectType*)>>(valueGetter);
			IReflectable& castDataType = f(castObjType);

			return castDataType;
		}

		virtual IReflectable& getArrayValue(void* object, UINT32 index)
		{
			checkIsArray(true);

			ObjectType* castObjType = static_cast<ObjectType*>(object);
			boost::function<DataType&(ObjectType*, UINT32)> f = boost::any_cast<boost::function<DataType&(ObjectType*, UINT32)>>(valueGetter);

			IReflectable& castDataType = f(castObjType, index);
			return castDataType;
		}

		virtual void setValue(void* object, IReflectable& value)
		{
			checkIsArray(false);

			if(valueSetter.empty())
			{
				CM_EXCEPT(InternalErrorException,
					"Specified field (" + mName + ") has no setter.");
			}

			ObjectType* castObjType = static_cast<ObjectType*>(object);
			DataType& castDataObj = static_cast<DataType&>(value);
			boost::function<void(ObjectType*, DataType&)> f = boost::any_cast<boost::function<void(ObjectType*, DataType&)>>(valueSetter);
			f(castObjType, castDataObj);
		}

		virtual void setArrayValue(void* object, UINT32 index, IReflectable& value)
		{
			checkIsArray(true);

			if(valueSetter.empty())
			{
				CM_EXCEPT(InternalErrorException, 
					"Specified field (" + mName + ") has no setter.");
			}

			ObjectType* castObjType = static_cast<ObjectType*>(object);
			DataType& castDataObj = static_cast<DataType&>(value);
			boost::function<void(ObjectType*, UINT32, DataType&)> f = boost::any_cast<boost::function<void(ObjectType*, UINT32, DataType&)>>(valueSetter);
			f(castObjType, index, castDataObj);
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

		virtual IReflectable* newObject()
		{
			return DataType::newObject();
		}
	};
}
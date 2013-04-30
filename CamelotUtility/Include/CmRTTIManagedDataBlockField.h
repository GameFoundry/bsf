#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmRTTIField.h"
#include "CmManagedDataBlock.h"

namespace CamelotFramework
{
	struct RTTIManagedDataBlockFieldBase : public RTTIField
	{
		boost::any mCustomAllocator;

		virtual ManagedDataBlock getValue(void* object) = 0;
		virtual void setValue(void* object, ManagedDataBlock value) = 0;

		virtual UINT8* allocate(void* object, UINT32 bytes) = 0;
	};

	template <class DataType, class ObjectType>
	struct RTTIManagedDataBlockField : public RTTIManagedDataBlockFieldBase
	{
		/**
		 * @brief	Initializes a field that returns a block of bytes. Can be used for serializing pretty much anything.
		 *
		 * @param	name		Name of the field.
		 * @param	uniqueId	Unique identifier for this field. Although name is also a unique
		 * 						identifier we want a small data type that can be used for efficiently
		 * 						serializing data to disk and similar. It is primarily used for compatibility
		 * 						between different versions of serialized data.
		 * @param	getter  	The getter method for the field. Cannot be null. Must be a specific signature: SerializableDataBlock(ObjectType*)
		 * @param	setter  	The setter method for the field. Can be null. Must be a specific signature: void(ObjectType*, SerializableDataBlock)	
		 * @param	flags		Various flags you can use to specialize how systems handle this field
		 * @param	customAllocator (optional) Custom allocator that will be used when de-serializing DataBlock memory.
		 */
		void initSingle(const std::string& name, UINT16 uniqueId, boost::any getter, boost::any setter, UINT64 flags, boost::any customAllocator = boost::any())
		{
			initAll(getter, setter, nullptr, nullptr, name, uniqueId, false, SerializableFT_DataBlock, flags);
			mCustomAllocator = customAllocator;
		}

		virtual UINT32 getTypeSize()
		{
			return 0; // Data block types don't store size the conventional way
		}

		virtual bool hasDynamicSize()
		{
			return true;
		}

		virtual UINT32 getArraySize(void* object)
		{
			CM_EXCEPT(InternalErrorException, 
				"Data block types don't support arrays.");
		}

		virtual void setArraySize(void* object, UINT32 size)
		{
			CM_EXCEPT(InternalErrorException, 
				"Data block types don't support arrays.");
		}

		virtual ManagedDataBlock getValue(void* object)
		{
			ObjectType* castObj = static_cast<ObjectType*>(object);
			boost::function<ManagedDataBlock(ObjectType*)> f = boost::any_cast<boost::function<ManagedDataBlock(ObjectType*)>>(valueGetter);
			return f(castObj);
		}

		virtual void setValue(void* object, ManagedDataBlock value)
		{
			ObjectType* castObj = static_cast<ObjectType*>(object);
			boost::function<void(ObjectType*, ManagedDataBlock)> f = boost::any_cast<boost::function<void(ObjectType*, ManagedDataBlock)>>(valueSetter);
			f(castObj, value);
		}

		virtual UINT8* allocate(void* object, UINT32 bytes)
		{
			if(mCustomAllocator.empty())
				return CM_NEW_BYTES(bytes, ScratchAlloc);
			else
			{
				ObjectType* castObj = static_cast<ObjectType*>(object);
				boost::function<UINT8*(ObjectType*, UINT32)> f = boost::any_cast<boost::function<UINT8*(ObjectType*, UINT32)>>(mCustomAllocator);
				return f(castObj, bytes);
			}
		}
	};
}
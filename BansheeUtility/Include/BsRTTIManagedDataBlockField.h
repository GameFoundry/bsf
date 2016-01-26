#pragma once

#include "BsPrerequisitesUtil.h"
#include "BsRTTIField.h"
#include "BsManagedDataBlock.h"

namespace BansheeEngine
{
	/**
	 * @brief	Base class containing common functionality for a managed data block class field. 
	 * 			
	 * @note	Managed data blocks are just blocks of memory that may, or may not be released
	 * 			automatically when they are no longer referenced. They are useful when wanting to
	 * 			return some temporary data only for serialization purposes.
	 */
	struct RTTIManagedDataBlockFieldBase : public RTTIField
	{
		Any mCustomAllocator;

		/**
		 * @brief	Retrieves a managed data block from the specified instance.
		 */
		virtual ManagedDataBlock getValue(void* object) = 0;

		/**
		 * @brief	Sets a managed data block on the specified instance.
		 */
		virtual void setValue(void* object, ManagedDataBlock value) = 0;

		/**
		 * @brief	Allocate memory for the managed data block. Used primarily
		 * 			to allocate memory before sending it to "setValue" method.
		 */
		virtual UINT8* allocate(void* object, UINT32 bytes) = 0;
	};

	/**
	 * @brief	Class containing a managed data block field containing a specific type.
	 */
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
		 * @param	getter  	The getter method for the field. Must be a specific signature: SerializableDataBlock(ObjectType*)
		 * @param	setter  	The setter method for the field. Must be a specific signature: void(ObjectType*, SerializableDataBlock)	
		 * @param	flags		Various flags you can use to specialize how systems handle this field. See "RTTIFieldFlag".
		 * @param	customAllocator (optional) Custom allocator that will be used when de-serializing DataBlock memory.
		 */
		void initSingle(const String& name, UINT16 uniqueId, Any getter, Any setter, UINT64 flags, Any customAllocator = Any())
		{
			initAll(getter, setter, nullptr, nullptr, name, uniqueId, false, SerializableFT_DataBlock, flags);
			mCustomAllocator = customAllocator;
		}

		/**
		 * @copydoc RTTIField::getTypeSize
		 */
		virtual UINT32 getTypeSize()
		{
			return 0; // Data block types don't store size the conventional way
		}

		/**
		 * @copydoc RTTIField::hasDynamicSize
		 */
		virtual bool hasDynamicSize()
		{
			return true;
		}

		/**
		 * @copydoc RTTIField::getArraySize
		 */
		virtual UINT32 getArraySize(void* object)
		{
			BS_EXCEPT(InternalErrorException, 
				"Data block types don't support arrays.");
		}

		/**
		 * @copydoc RTTIField::setArraySize
		 */
		virtual void setArraySize(void* object, UINT32 size)
		{
			BS_EXCEPT(InternalErrorException, 
				"Data block types don't support arrays.");
		}

		/**
		 * @copydoc RTTIManagedDataBlockFieldBase::getValue
		 */
		virtual ManagedDataBlock getValue(void* object)
		{
			ObjectType* castObj = static_cast<ObjectType*>(object);
			std::function<ManagedDataBlock(ObjectType*)> f = any_cast<std::function<ManagedDataBlock(ObjectType*)>>(valueGetter);
			return f(castObj);
		}

		/**
		 * @copydoc RTTIManagedDataBlockFieldBase::setValue
		 */
		virtual void setValue(void* object, ManagedDataBlock value)
		{
			ObjectType* castObj = static_cast<ObjectType*>(object);
			std::function<void(ObjectType*, ManagedDataBlock)> f = any_cast<std::function<void(ObjectType*, ManagedDataBlock)>>(valueSetter);
			f(castObj, value);
		}

		/**
		 * @copydoc RTTIManagedDataBlockFieldBase::allocate
		 */
		virtual UINT8* allocate(void* object, UINT32 bytes)
		{
			if(mCustomAllocator.empty())
				return (UINT8*)bs_alloc<ScratchAlloc>(bytes);
			else
			{
				ObjectType* castObj = static_cast<ObjectType*>(object);
				std::function<UINT8*(ObjectType*, UINT32)> f = any_cast<std::function<UINT8*(ObjectType*, UINT32)>>(mCustomAllocator);
				return f(castObj, bytes);
			}
		}
	};
}
#pragma once

#include "BsPrerequisitesUtil.h"

namespace BansheeEngine
{
	/** @addtogroup Serialization
	 *  @{
	 */

	/**
	 * Data block holding an array of bytes, usually used in serialization.
	 * 			
	 * Ownership of the data blocked is passed to the latest copy of the ManagedDataBlock. Data will be automatically 
	 * freed once the last copy is destroyed.
	 */
	class BS_UTILITY_EXPORT ManagedDataBlock
	{
	public:
		/**
		 * Constructor
		 *
		 * @param[in] 	data			Array of bytes to store. Direct pointer to the provided array will be stored, 
		 *								no copying will be done. 
		 * @param[in]	size			Size of the array, in bytes.
		 */
		ManagedDataBlock(UINT8* data, UINT32 size); 

		/**
		 * Constructor that will automatically allocate an internal buffer of the specified size. Copying ManagedDataBlock 
		 * transfers ownership of the buffer to the copy of the buffer. Buffer is deleted when the latest copy is deleted.
		 *
		 * @param[in]	size	The size of the data in bytes.
		 */
		ManagedDataBlock(UINT32 size);

		ManagedDataBlock(const ManagedDataBlock& source);

		~ManagedDataBlock();

		UINT8* getData() { return mData; }
		UINT32 getSize() { return mData ? mSize : 0; }

	private:
		UINT8* mData;
		UINT32 mSize;
		bool mManaged;
		mutable bool mIsDataOwner;
	};

	/** @} */
}
#pragma once

#include "CmPrerequisitesUtil.h"

namespace CamelotEngine
{
	/**
	 * @brief	Data block holding an array of bytes, usually used in serialization.
	 */
	class CM_UTILITY_EXPORT ManagedDataBlock
	{
	public:
		/**
		 * @brief	Constructor
		 *
		 * @param [in]	data		Array of bytes to store. Direct pointer to the provided
		 * 							array will be stored, no copying will be done. 
		 * @param	size			Size of the array, in bytes.
		 */
		ManagedDataBlock(UINT8* data, UINT32 size);

		/**
		 * @brief	Constructor that will automatically allocate an internal buffer of the specified size.
		 * 			Copying ManagedDataBlock transfers ownership of the buffer to the copy of the buffer.
		 * 			Buffer is deleted when the latest copy is deleted.
		 *
		 * @param	size	The size of the data in bytes.
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
}
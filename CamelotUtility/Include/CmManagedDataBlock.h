#pragma once

#include "CmPrerequisitesUtil.h"

namespace CamelotEngine
{
	/**
	 * @brief	Data block holding an array of bytes, usually used in serialization.
	 */
	class ManagedDataBlock
	{
	public:
		/**
		 * @brief	Constructor
		 *
		 * @param [in]	data		Array of bytes to store. Direct pointer to the provided
		 * 							array will be stored, no copying will be done. Data won't be modified
		 * 							unless "managed" is true.
		 * @param	size			Size of the array, in bytes.
		 * @param	managed			If true then the provided data array will be deleted once
		 * 							it is no longer being used. When serializing it is always faster to provide direct
		 * 							pointer to the data, but sometimes you need to provide a copy of the data
		 * 							instead (e.g. maybe it's needed in a different format). 
		 * 							In that case set managed to true so it can be properly freed.
		 */
		ManagedDataBlock(UINT8* data, UINT32 size, bool managed)
			:mData(data), mSize(size), mManaged(managed), mIsDataOwner(true)
		{ }

		ManagedDataBlock(const ManagedDataBlock& source)
		{
			mData = source.mData;
			mSize = source.mSize;
			mManaged = source.mManaged;

			mIsDataOwner = true;
			source.mIsDataOwner = false;
		}

		~ManagedDataBlock()
		{
			if(mManaged && mIsDataOwner)
				delete[] mData;
		}

		UINT8* getData() { return mData; }
		UINT32 getSize() { return mSize; }

	private:
		UINT8* mData;
		UINT32 mSize;
		bool mManaged;
		mutable bool mIsDataOwner;
	};
}
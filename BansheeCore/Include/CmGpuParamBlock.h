#pragma once

#include "CmPrerequisites.h"
#include "CmCoreObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Stores data (e.g. int, float, Vector2) GPU parameters in a raw buffer. 
	 *			Used primarily for caching GPU parameters on the CPU before they're submitted 
	 *			to the actual GPU parameter buffer.
	 */
	class BS_CORE_EXPORT GpuParamBlock
	{
	public:
		GpuParamBlock(UINT32 size);
		GpuParamBlock(GpuParamBlock* otherBlock);

		virtual ~GpuParamBlock();

		/**
		 * @brief	Write some data to the specified offset in the buffer. 
		 *			Marks the block as dirty.
		 *			All values are in bytes.
		 */
		void write(UINT32 offset, const void* data, UINT32 size);

		/**
		 * @brief	Read some data from the specified offset in the buffer.
		 *			All values are in bytes.
		 */
		void read(UINT32 offset, void* data, UINT32 size);

		/**
		 * @brief	Clear specified section of the buffer to zero.
		 *			All values are in bytes.
		 */
		void zeroOut(UINT32 offset, UINT32 size);

		/**
		 * @brief	Returns size of the internal buffer in bytes.
		 */
		UINT32 getSize() const { return mSize; }

		/**
		 * @brief	Returns a raw pointer to the internal buffer.
		 */
		UINT8* getData() const { return mData; }

		/**
		 * @brief	Checks if something has been written to the buffer
		 *			since the last time object was clean.
		 */
		bool isDirty() const { return mDirty; }

		/**
		 * @brief	Marks the object as dirty or clean. Signifies
		 *			whether or not some new data has been written in the buffer.
		 */
		void setDirty() { mDirty = true; }
	protected:
		UINT8* mData;
		UINT32 mSize;
		bool mDirty;
	};
}
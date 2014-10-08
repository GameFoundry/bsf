#pragma once

#include "BsCorePrerequisites.h"
#include "BsHardwareBuffer.h"
#include "BsCoreObject.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Hardware buffer that hold indices that reference vertices in a vertex buffer.
	 */
    class BS_CORE_EXPORT IndexBuffer : public HardwareBuffer, public CoreObject
    {
	public:
		/**
		 * @brief	Type of the indices used, used for determining size.
		 */
		enum IndexType 
		{
			IT_16BIT,
			IT_32BIT
		};

		~IndexBuffer();

		/**
		 * @brief	Returns the type of indices stored.
		 */
		IndexType getType() const { return mIndexType; }

		/**
		 * @brief	Returns the number of indices this buffer can hold.
		 */
		UINT32 getNumIndices() const { return mNumIndexes; }

		/**
		 * @brief	Returns the size of a single index in bytes.
		 */
		UINT32 getIndexSize() const { return mIndexSize; }

		/**
		 * @copydoc	HardwareBufferManager::createIndexBuffer
		 */
		static IndexBufferPtr create(IndexBuffer::IndexType itype, UINT32 numIndexes, GpuBufferUsage usage);

	protected:
		IndexBuffer(IndexType idxType, UINT32 numIndexes, GpuBufferUsage usage, bool useSystemMemory);

	protected:
		IndexType mIndexType;
		UINT32 mNumIndexes;
		UINT32 mIndexSize;
    };
}
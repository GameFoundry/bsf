#pragma once

#include "BsPrerequisitesUtil.h"

namespace BansheeEngine
{
	/**
	  * @brief	Encodes/decodes an IReflectable object from/to memory. 
	  */
	class BS_UTILITY_EXPORT MemorySerializer
	{
		struct BufferPiece
		{
			UINT8* buffer;
			UINT32 size;
		};

	public:
		MemorySerializer();
		~MemorySerializer();

		/**
		 * @brief	Parses the provided object, serializes all of its data as specified by its
		 *			RTTIType and returns the data in the form of raw memory.
		 *
		 *			All memory is allocated using the provided "allocator". If not specified the default
		 *			allocator is used.
		 */
		UINT8* encode(IReflectable* object, UINT32& bytesWritten, std::function<void*(UINT32)> allocator = nullptr);

		/**
		 * @brief	Deserializes an IReflectable object by reading the binary data from the provided
		 *			memory location.
		 */
		std::shared_ptr<IReflectable> decode(UINT8* buffer, UINT32 bufferSize);

	private:
		Vector<BufferPiece> mBufferPieces;

		/**
		 * @brief	Called by the binary serializer whenever the buffer gets full.
		 */
		UINT8* flushBuffer(UINT8* bufferStart, int bytesWritten, UINT32& newBufferSize);

		/************************************************************************/
		/* 								CONSTANTS	                     		*/
		/************************************************************************/
	private:
		static const UINT32 WRITE_BUFFER_SIZE = 2048;
	};
}
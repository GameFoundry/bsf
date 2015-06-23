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
		 * @param	object			Object to encode.
		 * @param	bytesWritten	Output value containing the total number of bytes it took to encode the object.
		 * @param	allocator		Determines how is memory allocated. If not specified the default allocator is used.
		 * @param	shallow			Determines how to handle referenced objects. If true then references will not be encoded
		 *							and will be set to null. If false then references will be encoded as well and restored
		 *							upon decoding.
		 *
		 * @return	A buffer containing the encoded object. It is up to the user to release the buffer memory when
		 *			no longer needed.
		 */
		UINT8* encode(IReflectable* object, UINT32& bytesWritten, std::function<void*(UINT32)> allocator = nullptr, 
			bool shallow = false);

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
		UINT8* flushBuffer(UINT8* bufferStart, UINT32 bytesWritten, UINT32& newBufferSize);

		/************************************************************************/
		/* 								CONSTANTS	                     		*/
		/************************************************************************/
	private:
		static const UINT32 WRITE_BUFFER_SIZE = 16384;
	};
}
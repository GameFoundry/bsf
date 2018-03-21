//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Serialization/BsMemorySerializer.h"

#include "Error/BsException.h"
#include "Reflection/BsIReflectable.h"
#include "Serialization/BsBinarySerializer.h"
#include "FileSystem/BsDataStream.h"

using namespace std::placeholders;

namespace bs
{
	UINT8* MemorySerializer::encode(IReflectable* object, UINT32& bytesWritten, 
		std::function<void*(UINT32)> allocator, bool shallow, const UnorderedMap<String, UINT64>& params)
	{
		using namespace std::placeholders;

		BinarySerializer bs;

		BufferPiece piece;
		piece.buffer = (UINT8*)bs_alloc(WRITE_BUFFER_SIZE);
		piece.size = 0;

		mBufferPieces.push_back(piece);

		bs.encode(object, piece.buffer, WRITE_BUFFER_SIZE, &bytesWritten, 
			std::bind(&MemorySerializer::flushBuffer, this, _1, _2, _3), shallow, params);

		UINT8* resultBuffer;
		if(allocator != nullptr)
			resultBuffer = (UINT8*)allocator(bytesWritten);
		else
			resultBuffer = (UINT8*)bs_alloc(bytesWritten);

		UINT32 offset = 0;
		for(auto iter = mBufferPieces.begin(); iter != mBufferPieces.end(); ++iter)
		{
			if(iter->size > 0)
			{
				memcpy(resultBuffer + offset, iter->buffer, iter->size);
				offset += iter->size;
			}
		}

		for(auto iter = mBufferPieces.rbegin(); iter != mBufferPieces.rend(); ++iter)
		{
			bs_free(iter->buffer);
		}

		mBufferPieces.clear();

		return resultBuffer;
	}

	SPtr<IReflectable> MemorySerializer::decode(UINT8* buffer, UINT32 bufferSize, const UnorderedMap<String, UINT64>& params)
	{
		SPtr<MemoryDataStream> stream = bs_shared_ptr_new<MemoryDataStream>(buffer, bufferSize, false);

		BinarySerializer bs;
		SPtr<IReflectable> object = bs.decode(stream, (UINT32)bufferSize, params);

		return object;
	}

	UINT8* MemorySerializer::flushBuffer(UINT8* bufferStart, UINT32 bytesWritten, UINT32& newBufferSize)
	{
		mBufferPieces.back().size = bytesWritten;

		BufferPiece piece;
		piece.buffer = (UINT8*)bs_alloc(WRITE_BUFFER_SIZE);
		piece.size = 0;

		mBufferPieces.push_back(piece);

		return piece.buffer;
	}
}

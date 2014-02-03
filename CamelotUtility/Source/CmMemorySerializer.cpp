#include "CmMemorySerializer.h"

#include "CmException.h"
#include "CmIReflectable.h"
#include "CmBinarySerializer.h"

#include <boost/bind.hpp>

namespace CamelotFramework
{
	MemorySerializer::MemorySerializer()
	{ }

	MemorySerializer::~MemorySerializer()
	{ }

	UINT8* MemorySerializer::encode(IReflectable* object, UINT32& bytesWritten, std::function<void*(UINT32)> allocator)
	{
		BinarySerializer bs;

		BufferPiece piece;
		piece.buffer = (UINT8*)stackAlloc(WRITE_BUFFER_SIZE);
		piece.size = 0;

		mBufferPieces.push_back(piece);

		bs.encode(object, piece.buffer, WRITE_BUFFER_SIZE, (INT32*)&bytesWritten, boost::bind(&MemorySerializer::flushBuffer, this, _1, _2, _3));

		UINT8* resultBuffer;
		if(allocator != nullptr)
			resultBuffer = (UINT8*)allocator(bytesWritten);
		else
			resultBuffer = (UINT8*)cm_alloc(bytesWritten);

		UINT32 offset = 0;
		for(auto iter = mBufferPieces.begin(); iter != mBufferPieces.end(); ++iter)
		{
			memcpy(resultBuffer + offset, iter->buffer, iter->size);
		}

		for(auto iter = mBufferPieces.rbegin(); iter != mBufferPieces.rend(); ++iter)
		{
			stackDeallocLast(iter->buffer);
		}

		return resultBuffer;
	}

	std::shared_ptr<IReflectable> MemorySerializer::decode(UINT8* buffer, UINT32 bufferSize)
	{
		BinarySerializer bs;
		std::shared_ptr<IReflectable> object = bs.decode(buffer, (UINT32)bufferSize);

		return object;
	}

	UINT8* MemorySerializer::flushBuffer(UINT8* bufferStart, int bytesWritten, UINT32& newBufferSize)
	{
		mBufferPieces.back().size = bytesWritten;

		BufferPiece piece;
		piece.buffer = (UINT8*)stackAlloc(WRITE_BUFFER_SIZE);
		piece.size = 0;

		mBufferPieces.push_back(piece);

		return piece.buffer;
	}
}
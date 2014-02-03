#pragma once

#include "CmPrerequisitesUtil.h"

namespace CamelotFramework
{
	class CM_UTILITY_EXPORT MemorySerializer
	{
		struct BufferPiece
		{
			UINT8* buffer;
			UINT32 size;
		};

	public:
		MemorySerializer();
		~MemorySerializer();

		UINT8* encode(IReflectable* object, UINT32& bytesWritten, std::function<void*(UINT32)> allocator = nullptr);
		std::shared_ptr<IReflectable> decode(UINT8* buffer, UINT32 bufferSize);

	private:
		Vector<BufferPiece>::type mBufferPieces;

		UINT8* flushBuffer(UINT8* bufferStart, int bytesWritten, UINT32& newBufferSize);

		/************************************************************************/
		/* 								CONSTANTS	                     		*/
		/************************************************************************/
	private:
		static const UINT32 WRITE_BUFFER_SIZE = 2048;
	};
}
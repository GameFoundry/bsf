#pragma once

#include "CmPrerequisitesUtil.h"

namespace CamelotFramework
{
	// TODO - Low priority. Eventually I'll want to generalize BinarySerializer to Serializer class, then I can make this class accept
	// a generic Serializer interface so it may write both binary, plain-text or some other form of data.
	class CM_UTILITY_EXPORT FileSerializer
	{
	public:
		FileSerializer();
		~FileSerializer();

		void encode(IReflectable* object, String fileLocation);
		std::shared_ptr<IReflectable> decode(String fileLocation);

	private:
		std::ofstream mOutputStream;
		UINT8* mWriteBuffer;

		std::ifstream mInputStream;

		UINT8* flushBuffer(UINT8* bufferStart, int bytesWritten, UINT32& newBufferSize);

		/************************************************************************/
		/* 								CONSTANTS	                     		*/
		/************************************************************************/
	private:
		static const UINT32 WRITE_BUFFER_SIZE = 2048;
	};
}
#include "CmFileSerializer.h"

#include "CmException.h"
#include "CmIReflectable.h"
#include "CmBinarySerializer.h"

#include <numeric>
#include <boost/bind.hpp>

namespace CamelotEngine
{
	FileSerializer::FileSerializer()
	{
		mWriteBuffer = new UINT8[WRITE_BUFFER_SIZE];
	}

	FileSerializer::~FileSerializer()
	{
		delete[] mWriteBuffer;
	}

	void FileSerializer::encode(IReflectable* object, std::string fileLocation)
	{
		mOutputStream.open(fileLocation.c_str(), std::ios::out | std::ios::binary);

		BinarySerializer bs;
		int totalBytesWritten = 0;
		bs.encode(object, mWriteBuffer, WRITE_BUFFER_SIZE, &totalBytesWritten, boost::bind(&FileSerializer::flushBuffer, this, _1, _2, _3));

		mOutputStream.close();
		mOutputStream.clear();
	}

	void FileSerializer::decode(IReflectable* object, std::string fileLocation)
	{
		mInputStream.open(fileLocation.c_str(), std::ios::in | std::ios::ate | std::ios::binary);
		
		std::streamoff fileSize = mInputStream.tellg();
		if(fileSize > std::numeric_limits<UINT32>::max())
		{
			CM_EXCEPT(InternalErrorException, 
				"File size is larger that UINT32 can hold. Ask a programmer to use a bigger data type.");
		}

		UINT8* readBuffer = new UINT8[(UINT32)fileSize]; // TODO - Low priority. Consider upgrading BinarySerializer so we don't have to read everything at once

		mInputStream.seekg(0, std::ios::beg);
		mInputStream.read((char*)readBuffer, fileSize);

		BinarySerializer bs;
		bs.decode(object, readBuffer, (UINT32)fileSize);

		mInputStream.close();
		mInputStream.clear();

		delete[] readBuffer;
	}

	UINT8* FileSerializer::flushBuffer(UINT8* bufferStart, int bytesWritten, UINT32& newBufferSize)
	{
		mOutputStream.write((const char*)bufferStart, bytesWritten);

		return bufferStart;
	}
}
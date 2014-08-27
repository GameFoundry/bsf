#include "BsFileSerializer.h"

#include "BsException.h"
#include "BsIReflectable.h"
#include "BsBinarySerializer.h"
#include "BsPath.h"
#include "BsFileSystem.h"

#include <numeric>

using namespace std::placeholders;

namespace BansheeEngine
{
	FileSerializer::FileSerializer()
	{
		mWriteBuffer = (UINT8*)bs_alloc<ScratchAlloc>(WRITE_BUFFER_SIZE);
	}

	FileSerializer::~FileSerializer()
	{
		bs_free<ScratchAlloc>(mWriteBuffer);
	}

	void FileSerializer::encode(IReflectable* object, const Path& fileLocation)
	{
		Path parentDir = fileLocation.getDirectory();
		if (!FileSystem::exists(parentDir))
			FileSystem::createDir(parentDir);

		mOutputStream.open(fileLocation.toString().c_str(), std::ios::out | std::ios::binary);

		BinarySerializer bs;
		int totalBytesWritten = 0;
		bs.encode(object, mWriteBuffer, WRITE_BUFFER_SIZE, &totalBytesWritten, std::bind(&FileSerializer::flushBuffer, this, _1, _2, _3));

		mOutputStream.close();
		mOutputStream.clear();
	}

	std::shared_ptr<IReflectable> FileSerializer::decode(const Path& fileLocation)
	{
		mInputStream.open(fileLocation.toString().c_str(), std::ios::in | std::ios::ate | std::ios::binary);
		
		std::streamoff fileSize = mInputStream.tellg();
		if(fileSize > std::numeric_limits<UINT32>::max())
		{
			BS_EXCEPT(InternalErrorException, 
				"File size is larger that UINT32 can hold. Ask a programmer to use a bigger data type.");
		}

		UINT8* readBuffer = (UINT8*)bs_alloc<ScratchAlloc>((UINT32)fileSize); // TODO - Low priority. Consider upgrading BinarySerializer so we don't have to read everything at once

		mInputStream.seekg(0, std::ios::beg);
		mInputStream.read((char*)readBuffer, fileSize);

		BinarySerializer bs;
		std::shared_ptr<IReflectable> object = bs.decode(readBuffer, (UINT32)fileSize);

		mInputStream.close();
		mInputStream.clear();

		bs_free<ScratchAlloc>(readBuffer);

		return object;
	}

	UINT8* FileSerializer::flushBuffer(UINT8* bufferStart, int bytesWritten, UINT32& newBufferSize)
	{
		mOutputStream.write((const char*)bufferStart, bytesWritten);

		return bufferStart;
	}
}
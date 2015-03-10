#include "BsFileSerializer.h"

#include "BsException.h"
#include "BsIReflectable.h"
#include "BsBinarySerializer.h"
#include "BsFileSystem.h"

#include <numeric>

using namespace std::placeholders;

namespace BansheeEngine
{
	FileEncoder::FileEncoder(const Path& fileLocation)
		:mWriteBuffer(nullptr)
	{
		mWriteBuffer = (UINT8*)bs_alloc<ScratchAlloc>(WRITE_BUFFER_SIZE);

		Path parentDir = fileLocation.getDirectory();
		if (!FileSystem::exists(parentDir))
			FileSystem::createDir(parentDir);

		mOutputStream.open(fileLocation.toString().c_str(), std::ios::out | std::ios::binary);
	}

	FileEncoder::~FileEncoder()
	{
		bs_free<ScratchAlloc>(mWriteBuffer);

		mOutputStream.close();
		mOutputStream.clear();
	}

	void FileEncoder::encode(IReflectable* object)
	{
		UINT64 curPos = (UINT64)mOutputStream.tellp();
		mOutputStream.seekp(sizeof(UINT32), std::ios_base::cur);

		BinarySerializer bs;
		int totalBytesWritten = 0;
		bs.encode(object, mWriteBuffer, WRITE_BUFFER_SIZE, &totalBytesWritten, std::bind(&FileEncoder::flushBuffer, this, _1, _2, _3));

		mOutputStream.seekp(curPos);
		mOutputStream.write((char*)&totalBytesWritten, sizeof(totalBytesWritten));
		mOutputStream.seekp(totalBytesWritten, std::ios_base::cur);
	}

	UINT8* FileEncoder::flushBuffer(UINT8* bufferStart, int bytesWritten, UINT32& newBufferSize)
	{
		mOutputStream.write((const char*)bufferStart, bytesWritten);

		return bufferStart;
	}

	FileDecoder::FileDecoder(const Path& fileLocation)
	{
		mInputStream.open(fileLocation.toString().c_str(), std::ios::in | std::ios::ate | std::ios::binary);

		std::streamoff fileSize = mInputStream.tellg();
		if (fileSize > std::numeric_limits<UINT32>::max())
		{
			BS_EXCEPT(InternalErrorException,
				"File size is larger that UINT32 can hold. Ask a programmer to use a bigger data type.");
		}

		mInputStream.seekg(0, std::ios::beg);
	}

	FileDecoder::~FileDecoder()
	{
		mInputStream.close();
		mInputStream.clear();
	}

	std::shared_ptr<IReflectable> FileDecoder::decode()
	{
		if (mInputStream.eof())
			return nullptr;

		UINT32 objectSize = 0;
		mInputStream.read((char*)&objectSize, sizeof(objectSize));

		UINT8* readBuffer = (UINT8*)bs_alloc<ScratchAlloc>((UINT32)objectSize); // TODO - Low priority. Consider upgrading BinarySerializer so we don't have to read everything at once
		mInputStream.read((char*)readBuffer, objectSize);

		BinarySerializer bs;
		std::shared_ptr<IReflectable> object = bs.decode(readBuffer, objectSize);

		bs_free<ScratchAlloc>(readBuffer);

		return object;
	}

	void FileDecoder::skip()
	{
		if (mInputStream.eof())
			return;

		UINT32 objectSize = 0;
		mInputStream.read((char*)&objectSize, sizeof(objectSize));

		mInputStream.seekg(objectSize, std::ios::cur);
	}
}
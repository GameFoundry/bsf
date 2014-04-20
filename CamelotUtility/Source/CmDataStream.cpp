#include "CmDataStream.h"
#include "CmDebug.h"
#include "CmException.h"

namespace BansheeEngine 
{
	const UINT32 DataStream::StreamTempSize = 128;

    template <typename T> DataStream& DataStream::operator>> (T& val)
    {
        read(static_cast<void*>(&val), sizeof(T));

        return *this;
    }

	String DataStream::getAsString()
	{
		// Read the entire buffer - ideally in one read, but if the size of
		// the buffer is unknown, do multiple fixed size reads.
		size_t bufSize = (mSize > 0 ? mSize : 4096);
		StringStream::char_type* tempBuffer = (StringStream::char_type*)cm_alloc(bufSize);

		// Ensure read from begin of stream
		seek(0);
		StringStream result;
		while (!eof())
		{
			size_t numReadBytes = read(tempBuffer, bufSize);
			result.write(tempBuffer, numReadBytes);
		}

		free(tempBuffer);
		return result.str();
	}

	WString DataStream::getAsWString()
	{
		// Read the entire buffer - ideally in one read, but if the size of
		// the buffer is unknown, do multiple fixed size reads.
		size_t bufSize = (mSize > 0 ? mSize : 4096);
		WStringStream::char_type* tempBuffer = (WStringStream::char_type*)cm_alloc(bufSize);

		// Ensure read from begin of stream
		seek(0);
		WStringStream result;
		while (!eof())
		{
			size_t numReadBytes = read(tempBuffer, bufSize);
			result.write(tempBuffer, numReadBytes);
		}

		free(tempBuffer);
		return result.str();
	}

    MemoryDataStream::MemoryDataStream(void* memory, size_t inSize)
		: DataStream(READ | WRITE), mData(nullptr)
    {
        mData = mPos = static_cast<UINT8*>(memory);
        mSize = inSize;
        mEnd = mData + mSize;

        assert(mEnd >= mPos);
    }

    MemoryDataStream::MemoryDataStream(DataStream& sourceStream)
        : DataStream(READ | WRITE), mData(nullptr)
    {
        // Copy data from incoming stream
        mSize = sourceStream.size();

		mData = (UINT8*)cm_alloc(sizeof(UINT8) * mSize);
		mPos = mData;
		mEnd = mData + sourceStream.read(mData, mSize);

        assert(mEnd >= mPos);
    }

    MemoryDataStream::MemoryDataStream(const DataStreamPtr& sourceStream)
        :DataStream(READ | WRITE), mData(nullptr)
    {
        // Copy data from incoming stream
        mSize = sourceStream->size();

		mData = (UINT8*)cm_alloc(sizeof(UINT8) * mSize);
		mPos = mData;
		mEnd = mData + sourceStream->read(mData, mSize);

        assert(mEnd >= mPos);
    }

    MemoryDataStream::~MemoryDataStream()
    {
        close();
    }

    size_t MemoryDataStream::read(void* buf, size_t count)
    {
        size_t cnt = count;

        if (mPos + cnt > mEnd)
            cnt = mEnd - mPos;
        if (cnt == 0)
            return 0;

        assert (cnt <= count);

        memcpy(buf, mPos, cnt);
        mPos += cnt;

        return cnt;
    }

	size_t MemoryDataStream::write(const void* buf, size_t count)
	{
		size_t written = 0;
		if (isWriteable())
		{
			written = count;

			if (mPos + written > mEnd)
				written = mEnd - mPos;
			if (written == 0)
				return 0;

			memcpy(mPos, buf, written);
			mPos += written;
		}

		return written;
	}

    void MemoryDataStream::skip(size_t count)
    {
        size_t newpos = (size_t)( (mPos - mData) + count );
        assert(mData + newpos <= mEnd);        

        mPos = mData + newpos;
    }

    void MemoryDataStream::seek(size_t pos)
    {
        assert(mData + pos <= mEnd);
        mPos = mData + pos;
    }

    size_t MemoryDataStream::tell() const
	{
		return mPos - mData;
	}

    bool MemoryDataStream::eof() const
    {
        return mPos >= mEnd;
    }

    void MemoryDataStream::close()    
    {
        if (mData != nullptr)
        {
            cm_free(mData);
            mData = nullptr;
        }
    }

    FileDataStream::FileDataStream(std::shared_ptr<std::ifstream> s, bool freeOnClose)
        : DataStream(READ), mpInStream(s), mpFStreamRO(s), mpFStream(0), mFreeOnClose(freeOnClose)
    {
        mpInStream->seekg(0, std::ios_base::end);
        mSize = (size_t)mpInStream->tellg();
        mpInStream->seekg(0, std::ios_base::beg);

		determineAccess();
    }

    FileDataStream::FileDataStream(std::shared_ptr<std::ifstream> s, size_t inSize, bool freeOnClose)
        : DataStream(READ), mpInStream(s), mpFStreamRO(s), mpFStream(0), mFreeOnClose(freeOnClose)
    {
        mSize = inSize;

		determineAccess();
    }

	FileDataStream::FileDataStream(std::shared_ptr<std::fstream> s, bool freeOnClose)
		: DataStream(READ | WRITE), mpInStream(s), mpFStreamRO(0), mpFStream(s), mFreeOnClose(freeOnClose)
	{
		mpInStream->seekg(0, std::ios_base::end);
		mSize = (size_t)mpInStream->tellg();
		mpInStream->seekg(0, std::ios_base::beg);

		determineAccess();
	}

	FileDataStream::FileDataStream(std::shared_ptr<std::fstream> s, size_t inSize, bool freeOnClose)
		: DataStream(READ | WRITE), mpInStream(s), mpFStreamRO(0), mpFStream(s), mFreeOnClose(freeOnClose)
	{
		mSize = inSize;

		determineAccess();
	}

	void FileDataStream::determineAccess()
	{
		mAccess = 0;

		if (mpInStream)
			mAccess |= READ;

		if (mpFStream)
			mAccess |= WRITE;
	}

    FileDataStream::~FileDataStream()
    {
        close();
    }

    size_t FileDataStream::read(void* buf, size_t count)
    {
		mpInStream->read(static_cast<char*>(buf), static_cast<std::streamsize>(count));

        return (size_t)mpInStream->gcount();
    }

	size_t FileDataStream::write(const void* buf, size_t count)
	{
		size_t written = 0;
		if (isWriteable() && mpFStream)
		{
			mpFStream->write(static_cast<const char*>(buf), static_cast<std::streamsize>(count));
			written = count;
		}

		return written;
	}
    void FileDataStream::skip(size_t count)
    {	
		mpInStream->clear(); // Clear fail status in case eof was set
		mpInStream->seekg(static_cast<std::ifstream::pos_type>(count), std::ios::cur);
    }

    void FileDataStream::seek(size_t pos)
    {
		mpInStream->clear(); // Clear fail status in case eof was set
		mpInStream->seekg(static_cast<std::streamoff>(pos), std::ios::beg);
	}

    size_t FileDataStream::tell() const
	{
		mpInStream->clear(); // Clear fail status in case eof was set

		return (size_t)mpInStream->tellg();
	}

    bool FileDataStream::eof() const
    {
        return mpInStream->eof();
    }

    void FileDataStream::close()
    {
        if (mpInStream)
        {
			if (mpFStreamRO)
	            mpFStreamRO->close();

			if (mpFStream)
			{
				mpFStream->flush();
				mpFStream->close();
			}

            if (mFreeOnClose)
            {
				mpInStream = nullptr;
				mpFStreamRO = nullptr; 
				mpFStream = nullptr; 
            }
        }
    }
}

#include "BsDataStream.h"
#include "BsDebug.h"
#include "BsException.h"
#include <codecvt>

namespace BansheeEngine 
{
	const UINT32 DataStream::StreamTempSize = 128;

	/**
	 * @brief	Checks does the provided buffer has an UTF32 byte order mark
	 *			in little endian order.
	 */
	bool isUTF32LE(const UINT8* buffer)
	{
		return buffer[0] == 0xFF && buffer[1] == 0xFE && buffer[2] == 0x00 && buffer[3] == 0x00;
	}

	/**
	 * @brief	Checks does the provided buffer has an UTF32 byte order mark
	 *			in big endian order.
	 */
	bool isUTF32BE(const UINT8* buffer)
	{
		return buffer[0] == 0x00 && buffer[1] == 0x00 && buffer[2] == 0xFE && buffer[3] == 0xFF;
	}

	/**
	 * @brief	Checks does the provided buffer has an UTF16 byte order mark
	 *			in little endian order.
	 */
	bool isUTF16LE(const UINT8* buffer)
	{
		return buffer[0] == 0xFF && buffer[1] == 0xFE;
	}

	/**
	 * @brief	Checks does the provided buffer has an UTF16 byte order mark
	 *			in big endian order.
	 */
	bool isUTF16BE(const UINT8* buffer)
	{
		return buffer[0] == 0xFE && buffer[1] == 0xFF;
	}

	/**
	 * @brief	Checks does the provided buffer has an UTF8 byte order mark.
	 */
	bool isUTF8(const UINT8* buffer)
	{
		return (buffer[0] == 0xEF && buffer[1] == 0xBB && buffer[2] == 0xBF);
	}

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
		std::stringstream::char_type* tempBuffer = (std::stringstream::char_type*)bs_alloc((UINT32)bufSize);

		// Ensure read from begin of stream
		seek(0);

		std::stringstream result;
		while (!eof())
		{
			size_t numReadBytes = read(tempBuffer, bufSize);
			result.write(tempBuffer, numReadBytes);
		}

		free(tempBuffer);
		std::string string = result.str();

		UINT32 readBytes = (UINT32)string.size();
		if (readBytes >= 4)
		{
			if (isUTF32LE((UINT8*)string.data()))
			{
				const std::codecvt_mode convMode = (std::codecvt_mode)(std::consume_header | std::little_endian);
				typedef std::codecvt_utf8<char32_t, 1114111, convMode> utf8utf32;

				std::wstring_convert<utf8utf32, char32_t> conversion("?");
				char32_t* start = (char32_t*)string.data();
				char32_t* end = (start + (string.size() - 1) / 4);

				return conversion.to_bytes(start, end).c_str();
			}
			else if (isUTF32BE((UINT8*)string.data()))
			{
				const std::codecvt_mode convMode = (std::codecvt_mode)(std::consume_header);
				typedef std::codecvt_utf8<char32_t, 1114111, convMode> utf8utf32;

				std::wstring_convert<utf8utf32, char32_t> conversion("?");
				char32_t* start = (char32_t*)string.data();
				char32_t* end = (start + (string.size() - 1) / 4);

				return conversion.to_bytes(start, end).c_str();
			}			
		}
		
		if (readBytes >= 3)
		{
			if (isUTF8((UINT8*)string.data()))
			{
				return string.c_str() + 3;
			}
		}

		if (readBytes >= 2)
		{
			if (isUTF16LE((UINT8*)string.data()))
			{
				const std::codecvt_mode convMode = (std::codecvt_mode)(std::little_endian);
				typedef std::codecvt_utf8<char16_t, 1114111, convMode> utf8utf16;

				std::wstring_convert<utf8utf16, char16_t> conversion("?");
				char16_t* start = (char16_t*)(string.data() + 2); // Bug?: std::consume_header seems to be ignored so I manually remove the header
				char16_t* end = (start + (string.size() - 1) / 2);

				return conversion.to_bytes(start, end).c_str();
			}
			else if (isUTF16BE((UINT8*)string.data()))
			{
				const std::codecvt_mode convMode = (std::codecvt_mode)(0);
				typedef std::codecvt_utf8<char16_t, 1114111, convMode> utf8utf16;

				// Bug?: Regardless of not providing the std::little_endian flag it seems that is how the data is read
				// so I manually flip it
				UINT32 numChars = (UINT32)(string.size() - 2) / 2;
				for (UINT32 i = 0; i < numChars; i++)
					std::swap(string[i * 2 + 0], string[i * 2 + 1]);

				std::wstring_convert<utf8utf16, char16_t> conversion("?");
				char16_t* start = (char16_t*)(string.data() + 2); // Bug?: std::consume_header seems to be ignored so I manually remove the header
				char16_t* end = (start + (string.size() - 1) / 2);

				return conversion.to_bytes(start, end).c_str();
			}
		}

		return string.c_str();
	}

	WString DataStream::getAsWString()
	{
		// Read the entire buffer - ideally in one read, but if the size of
		// the buffer is unknown, do multiple fixed size reads.
		size_t bufSize = (mSize > 0 ? mSize : 4096);
		std::stringstream::char_type* tempBuffer = (std::stringstream::char_type*)bs_alloc((UINT32)bufSize);

		// Ensure read from begin of stream
		seek(0);

		std::stringstream result;
		while (!eof())
		{
			size_t numReadBytes = read(tempBuffer, bufSize);
			result.write(tempBuffer, numReadBytes);
		}

		free(tempBuffer);
		std::string string = result.str();

		UINT32 readBytes = (UINT32)string.size();
		if (readBytes >= 4)
		{
			if (isUTF32LE((UINT8*)string.data()))
			{
				// Not supported
			}
			else if (isUTF32BE((UINT8*)string.data()))
			{
				// Not supported
			}
		}

		if (readBytes >= 3)
		{
			if (isUTF8((UINT8*)string.data()))
			{
				const std::codecvt_mode convMode = (std::codecvt_mode)(std::consume_header);
				typedef std::codecvt_utf8<wchar_t, 1114111, convMode> wcharutf8;

				std::wstring_convert<wcharutf8> conversion("?");
				return conversion.from_bytes(string).c_str();
			}
		}

		if (readBytes >= 2)
		{
			if (isUTF16LE((UINT8*)string.data()))
			{
				const std::codecvt_mode convMode = (std::codecvt_mode)(std::consume_header | std::little_endian);
				typedef std::codecvt_utf16 <wchar_t, 1114111, convMode> wcharutf16;

				std::wstring_convert<wcharutf16> conversion("?");
				return conversion.from_bytes(string).c_str();
			}
			else if (isUTF16BE((UINT8*)string.data()))
			{
				const std::codecvt_mode convMode = (std::codecvt_mode)(std::consume_header);
				typedef std::codecvt_utf16<wchar_t, 1114111, convMode> wcharutf16;

				std::wstring_convert<wcharutf16> conversion("?");
				return conversion.from_bytes(string).c_str();
			}
		}

		{
			const std::codecvt_mode convMode = (std::codecvt_mode)(std::consume_header);
			typedef std::codecvt_utf8<wchar_t, 1114111, convMode> wcharutf8;

			std::wstring_convert<wcharutf8> conversion("?");
			return conversion.from_bytes(string).c_str();
		}
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

		mData = (UINT8*)bs_alloc((UINT32)mSize);
		mPos = mData;
		mEnd = mData + sourceStream.read(mData, mSize);

        assert(mEnd >= mPos);
    }

    MemoryDataStream::MemoryDataStream(const DataStreamPtr& sourceStream)
        :DataStream(READ | WRITE), mData(nullptr)
    {
        // Copy data from incoming stream
        mSize = sourceStream->size();

		mData = (UINT8*)bs_alloc((UINT32)mSize);
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
            bs_free(mData);
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

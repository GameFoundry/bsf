#pragma once

#include "CmPrerequisitesUtil.h"
#include <istream>

namespace BansheeEngine 
{
	/**
	 * @brief	General purpose class used for encapsulating the reading and writing of data from
	 *			and to various sources using a common interface.
	 */
	class BS_UTILITY_EXPORT DataStream
	{
	public:
		enum AccessMode
		{
			READ = 1, 
			WRITE = 2
		};

	public:
        /**
         * @brief	Creates an unnamed stream.
         */
        DataStream(UINT16 accessMode = READ) 
			:mSize(0), mAccess(accessMode) 
		{ }

        /**
         * @brief	Creates a named stream.
         */
		DataStream(const String& name, UINT16 accessMode = READ) 
			:mName(name), mSize(0), mAccess(accessMode) {}

		virtual ~DataStream() {}

		const String& getName(void) { return mName; }
		UINT16 getAccessMode() const { return mAccess; }

		virtual bool isReadable() const { return (mAccess & READ) != 0; }
		virtual bool isWriteable() const { return (mAccess & WRITE) != 0; }
       
        /**
         * @brief	Reads data from the buffer and copies it to the specified value.
         */
        template<typename T> DataStream& operator>>(T& val);

		/**
		 * @brief	Read the requisite number of bytes from the stream,
		 *			stopping at the end of the file.
		 *
		 * @param	buf		Pre-allocated buffer to read the data into.
		 * @param	count	Number of bytes to read.
		 *
		 * @return	Number of bytes actually read.
		 * 			
		 * @note	Stream must be created with READ access mode.
		 */
		virtual size_t read(void* buf, size_t count) = 0;

		/**
		 * @brief	Write the requisite number of bytes to the stream.
		 *
		 * @param	buf		Buffer containing bytes to write.
		 * @param	count	Number of bytes to write.
		 *
		 * @return	Number of bytes actually written.
		 * 			
		 * @note	Stream must be created with WRITE access mode.
		 */
		virtual size_t write(const void* buf, size_t count) { return 0; }

	    /**
	     * @brief	Returns a string containing the entire stream.
	     *
		 * @note	 This is a convenience method for text streams only, allowing you to
		 *			retrieve a String object containing all the data in the stream.
	     */
	    virtual String getAsString();

	    /**
	     * @brief	Returns a wide string containing the entire stream.
	     *
		 * @note	This is a convenience method for text streams only, allowing you to
		 *			retrieve a WString object containing all the data in the stream.
	     */
	    virtual WString getAsWString();

		/**
		 * @brief	Skip a defined number of bytes. This can also be a negative value, in which case
		 *			the file pointer rewinds a defined number of bytes.
		 */
		virtual void skip(size_t count) = 0;
	
	    /**
	     * @brief	Repositions the read point to a specified byte.
	     */
	    virtual void seek(size_t pos) = 0;
		
	    /**
	     * @brief	Returns the current byte offset from beginning
	     */
	    virtual size_t tell() const = 0;

	    /**
	     * @brief	Returns true if the stream has reached the end.
	     */
	    virtual bool eof() const = 0;

        /**
		 * @brief	Returns the total size of the data to be read from the stream,
		 *			or 0 if this is indeterminate for this stream.
         */
        size_t size() const { return mSize; }

        /**
         * @brief	Close the stream. This makes further operations invalid.
         */
        virtual void close() = 0;
		
	protected:
		static const UINT32 StreamTempSize;

		String mName;		
        size_t mSize;
		UINT16 mAccess;
	};

	/**
	 * @brief	Data stream for handling data from memory.
	 */
	class BS_UTILITY_EXPORT MemoryDataStream : public DataStream
	{		
	public:
		/**
		 * @brief	Wrap an existing memory chunk in a stream.
		 *
		 * @param 	memory		Memory to wrap the data stream around.
		 * @param	size		Size of the memory chunk in bytes.
		 */
		MemoryDataStream(void* memory, size_t size);
		
		/**
		 * @brief	Create a stream which pre-buffers the contents of another stream. Data
		 * 			from the other buffer will be entirely read and stored in an internal buffer.
		 *
		 * @param [in]	sourceStream		Stream to read data from.
		 */
		MemoryDataStream(DataStream& sourceStream);
		
		/**
		 * @brief	Create a stream which pre-buffers the contents of another stream. Data
		 * 			from the other buffer will be entirely read and stored in an internal buffer.
		 *
		 * @param [in]	sourceStream		Stream to read data from.
		 */
		MemoryDataStream(const DataStreamPtr& sourceStream);

		~MemoryDataStream();

		/**
		 * @brief	Get a pointer to the start of the memory block this stream holds.
		 */
		UINT8* getPtr() { return mData; }
		
		/**
		 * @brief	Get a pointer to the current position in the memory block this stream holds.
		 */
		UINT8* getCurrentPtr() { return mPos; }
		
        /** 
		 * @copydoc DataStream::read
         */
		size_t read(void* buf, size_t count);

        /** 
		 * @copydoc DataStream::write
         */
		size_t write(const void* buf, size_t count);

        /** 
		 * @copydoc DataStream::skip
         */
		void skip(size_t count);
	
        /** 
		 * @copydoc DataStream::seek
         */
	    void seek( size_t pos );
		
        /** 
		 * @copydoc DataStream::tell
         */
	    size_t tell(void) const;

        /** 
		 * @copydoc DataStream::eof
         */
	    bool eof(void) const;

        /** 
		 * @copydoc DataStream::close
         */
        void close(void);

	protected:
		UINT8* mData;
		UINT8* mPos;
		UINT8* mEnd;

		bool mFreeOnClose;
	};

	/**
	 * @brief	Data stream for handling data from standard streams.
	 */
	class BS_UTILITY_EXPORT FileDataStream : public DataStream
	{
	public:
		/**
		 * @brief	Construct read-only stream from an standard stream.
		 * 			
		 *			If "freeOnClose" is true, the STL stream will be freed once the data stream is closed.
		 */
		FileDataStream(std::shared_ptr<std::ifstream> s, bool freeOnClose = true);

		/**
		 * @brief	Construct read-write stream from an standard stream.
		 * 			
		 *			If "freeOnClose" is true, the STL stream will be freed once the data stream is closed.
		 */
		FileDataStream(std::shared_ptr<std::fstream> s, bool freeOnClose = true);

		/**
		 * @brief	Construct read-only stream from an standard stream, and tell it the size.
		 * 			
		 *			Size parameter allows you to specify the size without requiring us to seek to the end of the stream
		 *			to find the size.
		 *			
		 *			If "freeOnClose" is true, the STL stream will be freed once the data stream is closed.
		 */
		FileDataStream(std::shared_ptr<std::ifstream> s, size_t size, bool freeOnClose = true);

		/**
		 * @brief	Construct read-write stream from an standard stream, and tell it the size.
		 * 			
		 *			Size parameter allows you to specify the size without requiring us to seek to the end of the stream
		 *			to find the size.
		 *			
		 *			If "freeOnClose" is true, the STL stream will be freed once the data stream is closed.
		 */
		FileDataStream(std::shared_ptr<std::fstream> s, size_t size, bool freeOnClose = true);

		~FileDataStream();

        /** 
		 * @copydoc DataStream::read
         */
		size_t read(void* buf, size_t count);

        /** 
		 * @copydoc DataStream::write
         */
		size_t write(const void* buf, size_t count);

        /** 
		 * @copydoc DataStream::skip
         */
		void skip(size_t count);
	
        /** 
		 * @copydoc DataStream::seek
         */
	    void seek(size_t pos);

        /** 
		 * @copydoc DataStream::tell
         */
		size_t tell() const;

        /** 
		 * @copydoc DataStream::eof
         */
	    bool eof() const;

        /** 
		 * @copydoc DataStream::close
         */
        void close();

	protected:
		std::shared_ptr<std::istream> mpInStream;
		std::shared_ptr<std::ifstream> mpFStreamRO;
		std::shared_ptr<std::fstream> mpFStream;
		bool mFreeOnClose;	

		void determineAccess();
	};
}


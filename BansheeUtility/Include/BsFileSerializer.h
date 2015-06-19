#pragma once

#include "BsPrerequisitesUtil.h"

namespace BansheeEngine
{
	// TODO - Low priority. Eventually I'll want to generalize BinarySerializer to Serializer class, then I can make this class accept
	// a generic Serializer interface so it may write both binary, plain-text or some other form of data.

	/**
	 * @brief	Encodes the provided object to the specified file using the RTTI system.
	 */
	class BS_UTILITY_EXPORT FileEncoder
	{
	public:
		FileEncoder(const Path& fileLocation);
		~FileEncoder();

		/**
		 * @brief	Parses the provided object, serializes all of its data as specified by its
		 *			RTTIType and saves the serialized data to the provided file location.
		 */
		void encode(IReflectable* object);

	private:
		/**
		 * @brief	Called by the binary serializer whenever the buffer gets full.
		 */
		UINT8* flushBuffer(UINT8* bufferStart, UINT32 bytesWritten, UINT32& newBufferSize);

		std::ofstream mOutputStream;
		UINT8* mWriteBuffer;

		static const UINT32 WRITE_BUFFER_SIZE = 2048;
	};

	/**
	 * @brief	Decodes objects from the specified file using the RTTI system.
	 */
	class BS_UTILITY_EXPORT FileDecoder
	{
	public:
		FileDecoder(const Path& fileLocation);
		~FileDecoder();

		/**
		 * @brief	Deserializes an IReflectable object by reading the binary data at
		 *			the provided file location.
		 */
		std::shared_ptr<IReflectable> decode();

		/**
		 * @brief	Skips over than object in the file. Calling "decode" will decode
		 *			the next object.
		 */
		void skip();

	private:
		std::ifstream mInputStream;
	};
}
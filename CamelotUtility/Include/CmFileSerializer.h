#pragma once

#include "CmPrerequisitesUtil.h"

namespace BansheeEngine
{
	// TODO - Low priority. Eventually I'll want to generalize BinarySerializer to Serializer class, then I can make this class accept
	// a generic Serializer interface so it may write both binary, plain-text or some other form of data.

	/**
	  * @brief	Encodes/decodes an IReflectable object from/to a file. 
	  */
	class CM_UTILITY_EXPORT FileSerializer
	{
	public:
		FileSerializer();
		~FileSerializer();

		/**
		 * @brief	Parses the provided object, serializes all of its data as specified by its
		 *			RTTIType and saves the serialized data to the provided file location.
		 */
		void encode(IReflectable* object, const Path& fileLocation);

		/**
		 * @brief	Deserializes an IReflectable object by reading the binary data at
		 *			the provided file location.
		 */
		std::shared_ptr<IReflectable> decode(const Path& fileLocation);

	private:
		std::ofstream mOutputStream;
		UINT8* mWriteBuffer;

		std::ifstream mInputStream;

		/**
		 * @brief	Called by the binary serializer whenever the buffer gets full.
		 */
		UINT8* flushBuffer(UINT8* bufferStart, int bytesWritten, UINT32& newBufferSize);

		/************************************************************************/
		/* 								CONSTANTS	                     		*/
		/************************************************************************/
	private:
		static const UINT32 WRITE_BUFFER_SIZE = 2048;
	};
}
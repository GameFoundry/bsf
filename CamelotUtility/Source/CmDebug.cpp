#include "CmDebug.h"
#include "CmLog.h"
#include "CmException.h"
#include "CmBitmapWriter.h"
#include "CmFileSystem.h"
#include "CmDataStream.h"
#include "CmPixelData.h"
#include "CmPixelUtil.h"

namespace CamelotFramework
{
	void Debug::logDebug(std::string msg)
	{
		mLog.logMsg(msg, "GlobalDebug");
	}

	void Debug::logInfo(std::string msg)
	{
		mLog.logMsg(msg, "GlobalInfo");
	}

	void Debug::logWarning(std::string msg)
	{
		mLog.logMsg(msg, "GlobalWarning");
	}

	void Debug::logError(std::string msg)
	{
		mLog.logMsg(msg, "GlobalError");
	}

	void Debug::log(const String& msg, const String& channel)
	{
		mLog.logMsg(msg, channel);
	}

	void Debug::writeAsBMP(const PixelData& pixelData, const String& filePath, bool overwrite) const
	{
		if(FileSystem::fileExists(filePath))
		{
			if(overwrite)
				FileSystem::remove(filePath);
			else
				CM_EXCEPT(FileNotFoundException, "File already exists at specified location: " + filePath);
		}

		DataStreamPtr ds = FileSystem::create(filePath);

		UINT32 bytesPerPixel = PixelUtil::getNumElemBytes(pixelData.format);

		UINT32 bmpDataSize = BitmapWriter::getBMPSize(pixelData.getWidth(), pixelData.getHeight(), bytesPerPixel);
		UINT8* bmpBuffer = CM_NEW_ARRAY(UINT8, bmpDataSize, ScratchAlloc);

		BitmapWriter::rawPixelsToBMP((UINT8*)pixelData.data, bmpBuffer, pixelData.getWidth(), pixelData.getHeight(), bytesPerPixel);

		ds->write(bmpBuffer, bmpDataSize);
		ds->close();

		CM_DELETE_ARRAY(bmpBuffer, UINT8, bmpDataSize, ScratchAlloc);
	}

	CM_UTILITY_EXPORT Debug& gDebug()
	{
		static Debug debug;
		return debug;
	}
}
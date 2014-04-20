#include "CmDebug.h"
#include "CmLog.h"
#include "CmException.h"
#include "CmBitmapWriter.h"
#include "CmFileSystem.h"
#include "CmDataStream.h"

namespace BansheeEngine
{
	void Debug::logDebug(const String& msg)
	{
		mLog.logMsg(msg, "GlobalDebug");
	}

	void Debug::logInfo(const String& msg)
	{
		mLog.logMsg(msg, "GlobalInfo");
	}

	void Debug::logWarning(const String& msg)
	{
		mLog.logMsg(msg, "GlobalWarning");
	}

	void Debug::logError(const String& msg)
	{
		mLog.logMsg(msg, "GlobalError");
	}

	void Debug::log(const String& msg, const String& channel)
	{
		mLog.logMsg(msg, channel);
	}

	void Debug::writeAsBMP(UINT8* rawPixels, UINT32 bytesPerPixel, UINT32 width, UINT32 height, const WString& filePath, bool overwrite) const
	{
		if(FileSystem::isFile(filePath))
		{
			if(overwrite)
				FileSystem::remove(filePath);
			else
				CM_EXCEPT(FileNotFoundException, "File already exists at specified location: " + toString(filePath));
		}

		DataStreamPtr ds = FileSystem::createAndOpenFile(filePath);

		UINT32 bmpDataSize = BitmapWriter::getBMPSize(width, height, bytesPerPixel);
		UINT8* bmpBuffer = cm_newN<UINT8, ScratchAlloc>(bmpDataSize);

		BitmapWriter::rawPixelsToBMP(rawPixels, bmpBuffer, width, height, bytesPerPixel);

		ds->write(bmpBuffer, bmpDataSize);
		ds->close();

		cm_deleteN<ScratchAlloc>(bmpBuffer, bmpDataSize);
	}

	CM_UTILITY_EXPORT Debug& gDebug()
	{
		static Debug debug;
		return debug;
	}
}
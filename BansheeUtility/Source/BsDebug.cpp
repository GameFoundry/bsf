#include "BsDebug.h"
#include "BsLog.h"
#include "BsException.h"
#include "BsBitmapWriter.h"
#include "BsFileSystem.h"
#include "BsDataStream.h"

#if BS_PLATFORM == BS_PLATFORM_WIN32 && BS_COMPILER == BS_COMPILER_MSVC
#include <windows.h>
#include <iostream>

void logToIDEConsole(const BansheeEngine::String& message)
{
	OutputDebugString(message.c_str());
	OutputDebugString("\n");

	// Also default output in case we're running without debugger attached
	std::cout << message << std::endl;
}
#else
void logToIDEConsole(const BansheeEngine::String& message)
{
	// Do nothing
}
#endif

namespace BansheeEngine
{
	void Debug::logDebug(const String& msg)
	{
		mLog.logMsg(msg, (UINT32)DebugChannel::Debug);
		logToIDEConsole(msg);
	}

	void Debug::logInfo(const String& msg)
	{
		mLog.logMsg(msg, (UINT32)DebugChannel::Info);
		logToIDEConsole(msg);
	}

	void Debug::logWarning(const String& msg)
	{
		mLog.logMsg(msg, (UINT32)DebugChannel::Warning);
		logToIDEConsole(msg);
	}

	void Debug::logError(const String& msg)
	{
		mLog.logMsg(msg, (UINT32)DebugChannel::Error);
		logToIDEConsole(msg);
	}

	void Debug::log(const String& msg, UINT32 channel)
	{
		mLog.logMsg(msg, channel);
		logToIDEConsole(msg);
	}

	void Debug::writeAsBMP(UINT8* rawPixels, UINT32 bytesPerPixel, UINT32 width, UINT32 height, const Path& filePath, bool overwrite) const
	{
		if(FileSystem::isFile(filePath))
		{
			if(overwrite)
				FileSystem::remove(filePath);
			else
				BS_EXCEPT(FileNotFoundException, "File already exists at specified location: " + filePath.toString());
		}

		DataStreamPtr ds = FileSystem::createAndOpenFile(filePath);

		UINT32 bmpDataSize = BitmapWriter::getBMPSize(width, height, bytesPerPixel);
		UINT8* bmpBuffer = bs_newN<UINT8>(bmpDataSize);

		BitmapWriter::rawPixelsToBMP(rawPixels, bmpBuffer, width, height, bytesPerPixel);

		ds->write(bmpBuffer, bmpDataSize);
		ds->close();

		bs_deleteN(bmpBuffer, bmpDataSize);
	}

	BS_UTILITY_EXPORT Debug& gDebug()
	{
		static Debug debug;
		return debug;
	}
}
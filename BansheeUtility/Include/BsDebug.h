#pragma once

#include "BsPrerequisitesUtil.h"
#include "BsLog.h"

namespace BansheeEngine
{
	class Log;

	/**
	 * @brief	Available types of channels that debug messages can be logged to.
	 */
	enum class DebugChannel
	{
		Debug, Warning, Error
	};

	/**
	 * @brief	Utility class providing various debug functionality. Thread safe.
	 */
	class BS_UTILITY_EXPORT Debug
	{
	public:
		/**
		 * @brief	Adds a log entry in the "Debug" channel.
		 */
		void logDebug(const String& msg);

		/**
		 * @brief	Adds a log entry in the "Warning" channel.
		 */
		void logWarning(const String& msg);

		/**
		 * @brief	Adds a log entry in the "Error" channel.
		 */
		void logError(const String& msg);

		/**
		 * @brief	Adds a log entry in the specified channel. You may specify custom channels as needed.
		 */
		void log(const String& msg, UINT32 channel);

		/**
		 * @brief	Retrieves the Log used by the Debug instance.
		 */
		Log& getLog() { return mLog; }

		/**
		 * @brief	Converts raw pixels into a BMP image. See "BitmapWriter" for more information.
		 */
		void writeAsBMP(UINT8* rawPixels, UINT32 bytesPerPixel, UINT32 width, UINT32 height, const Path& filePath, bool overwrite = true) const;

		/**
		 * @brief	Saves a log about the current state of the application to the specified location.
		 * 
		 * @param	path	Absolute path to the log filename.
		 */
		void saveLog(const Path& path);
	private:
		Log mLog;
	};

	BS_UTILITY_EXPORT Debug& gDebug();

#define LOGDBG(x) BansheeEngine::gDebug().logDebug((x));
#define LOGWRN(x) BansheeEngine::gDebug().logWarning((x));
#define LOGERR(x) BansheeEngine::gDebug().logError((x));
}
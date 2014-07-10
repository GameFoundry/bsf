//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsPrerequisitesUtil.h"
#include "BsLog.h"

namespace BansheeEngine
{
	class Log;

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
		 * @brief	Adds a log entry in the "Info" channel.
		 */
		void logInfo(const String& msg);

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
		void log(const String& msg, const String& channel);

		/**
		 * @brief	Retrieves the Log used by the Debug instance.
		 */
		Log& getLog() { return mLog; }

		/**
		 * @brief	Converts raw pixels into a BMP image. See "BitmapWriter" for more information.
		 */
		void writeAsBMP(UINT8* rawPixels, UINT32 bytesPerPixel, UINT32 width, UINT32 height, const Path& filePath, bool overwrite = true) const;

	private:
		Log mLog;
	};

	BS_UTILITY_EXPORT Debug& gDebug();

#define LOGDBG(x) BansheeEngine::gDebug().logDebug((x));
#define LOGINFO(x) BansheeEngine::gDebug().logInfo((x));
#define LOGWRN(x) BansheeEngine::gDebug().logWarning((x));
#define LOGERR(x) BansheeEngine::gDebug().logError((x));
}
#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmLog.h"

namespace CamelotFramework
{
	class Log;

	class CM_UTILITY_EXPORT Debug
	{
	public:
		void logDebug(std::string msg);
		void logInfo(std::string msg);
		void logWarning(std::string msg);
		void logError(std::string msg);
		void log(const String& msg, const String& channel);

		Log& getLog() { return mLog; }

		void writeAsBMP(UINT8* rawPixels, UINT32 bytesPerPixel, UINT32 width, UINT32 height, const String& filePath, bool overwrite = true) const;

	private:
		Log mLog;
	};

	CM_UTILITY_EXPORT Debug& gDebug();

	template <typename T>
	std::string toStr(T number)
	{
		std::ostringstream ss;
		ss << number;
		return ss.str();
	}

#define LOGDBG(x) CamelotFramework::gDebug().logDebug((x));
#define LOGINFO(x) CamelotFramework::gDebug().logInfo((x));
#define LOGWRN(x) CamelotFramework::gDebug().logWarning((x));
#define LOGERR(x) CamelotFramework::gDebug().logError((x));

#define CM_ASSERT(x) assert(x)
}
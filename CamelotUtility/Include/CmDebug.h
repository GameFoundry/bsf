#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmLog.h"

namespace CamelotFramework
{
	class Log;

	class CM_UTILITY_EXPORT Debug
	{
	public:
		void logDebug(const String& msg);
		void logInfo(const String& msg);
		void logWarning(const String& msg);
		void logError(const String& msg);
		void log(const String& msg, const String& channel);

		Log& getLog() { return mLog; }

		void writeAsBMP(UINT8* rawPixels, UINT32 bytesPerPixel, UINT32 width, UINT32 height, const WString& filePath, bool overwrite = true) const;

	private:
		Log mLog;
	};

	CM_UTILITY_EXPORT Debug& gDebug();

#define LOGDBG(x) CamelotFramework::gDebug().logDebug((x));
#define LOGINFO(x) CamelotFramework::gDebug().logInfo((x));
#define LOGWRN(x) CamelotFramework::gDebug().logWarning((x));
#define LOGERR(x) CamelotFramework::gDebug().logError((x));

#define CM_ASSERT(x) assert(x)
}
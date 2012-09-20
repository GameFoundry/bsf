#include "CmDebug.h"
#include "CmLog.h"

namespace CamelotEngine
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

	CM_UTILITY_EXPORT Debug& gDebug()
	{
		static Debug debug;
		return debug;
	}
}
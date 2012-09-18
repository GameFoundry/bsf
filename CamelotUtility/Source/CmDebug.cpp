#include "CmDebug.h"
#include "CmLog.h"

namespace CamelotEngine
{
	void Debug::logDebug(std::string msg)
	{
		mLog.logMsg(msg, LL_DEBUG);
	}

	void Debug::logInfo(std::string msg)
	{
		mLog.logMsg(msg, LL_INFO);
	}

	void Debug::logWarning(std::string msg)
	{
		mLog.logMsg(msg, LL_WARNING);
	}

	void Debug::logError(std::string msg)
	{
		mLog.logMsg(msg, LL_ERROR);
	}

	CM_EXPORT Debug& gDebug()
	{
		static Debug debug;
		return debug;
	}
}
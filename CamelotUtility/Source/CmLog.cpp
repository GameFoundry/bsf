#include "CmLog.h"
#include "CmException.h"
#include "boost/signal.hpp"

namespace CamelotFramework
{
	LogEntry::LogEntry(const String& msg, const String& level)
		:mMsg(msg), mChannel(level)
	{ }

	Log::Log()
	{
	}

	Log::~Log()
	{
		for(auto iter = mEntries.begin(); iter != mEntries.end(); ++iter)
			cm_delete<PoolAlloc>(*iter);
	}

	void Log::logMsg(const String& message, const String& level)
	{
		LogEntry* newEntry = cm_new<LogEntry, PoolAlloc>(message, level);
		mEntries.push_back(newEntry);

		doOnEntryAdded(*newEntry);
	}

	void Log::clear()
	{
		for(auto iter = mEntries.begin(); iter != mEntries.end(); ++iter)
			cm_delete<PoolAlloc>(*iter);

		mEntries.clear();
	}

	void Log::saveToFile(const WString& path)
	{
		// TODO - Save the log as HTML
		CM_EXCEPT(NotImplementedException, "Log save to file not yet implemented.");
	}

	void Log::doOnEntryAdded(const LogEntry& entry)
	{
		onEntryAdded(entry);
	}
}
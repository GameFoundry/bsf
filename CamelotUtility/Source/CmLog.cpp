#include "CmLog.h"
#include "boost/signal.hpp"

namespace CamelotFramework
{
	LogEntry::LogEntry(const String& msg, const String& level)
		:mMsg(msg), mChannel(level)
	{ }

	Log::Log(const String& logFilePath, bool autoSave, bool suppressFileOutput)
		:mAutoSave(autoSave), mSuppressFileOutput(suppressFileOutput), mSaveFilePath(logFilePath)
	{
	}

	Log::~Log()
	{
		flush();

		for(auto iter = mEntries.begin(); iter != mEntries.end(); ++iter)
			cm_delete<PoolAlloc>(*iter);
	}

	void Log::logMsg(const String& message, const String& level)
	{
		LogEntry* newEntry = cm_new<LogEntry, PoolAlloc>(message, level);
		mEntries.push_back(newEntry);

		doOnEntryAdded(*newEntry);

		if(mAutoSave)
			flush();
	}

	void Log::clear()
	{
		for(auto iter = mEntries.begin(); iter != mEntries.end(); ++iter)
			cm_delete<PoolAlloc>(*iter);

		mEntries.clear();

		if(mAutoSave)
			flush();
	}

	void Log::flush()
	{
		if(mSuppressFileOutput)
			return;

		// TODO - Write to disk and throw exception if it can't
		
		//OGRE_EXCEPT(::Ogre::Exception::ERR_INTERNAL_ERROR, 
		//	"Cannot create log file at " + mSaveFilePath,
		//	"Log::save()");
	}

	void Log::doOnEntryAdded(const LogEntry& entry)
	{
		onEntryAdded(entry);
	}
}

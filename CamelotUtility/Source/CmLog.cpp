#include "CmLog.h"

namespace CamelotEngine
{
	LogEntry::LogEntry(const String& msg, LogLevel level)
		:mMsg(msg), mLevel(level)
	{ }

	Log::Log(const String& logFilePath, bool autoSave, bool suppressFileOutput)
		:mAutoSave(autoSave), mSuppressFileOutput(suppressFileOutput), mSaveFilePath(logFilePath)
	{
	}

	Log::~Log()
	{
		save();

		for(auto iter = mEntries.begin(); iter != mEntries.end(); ++iter)
			delete *iter;
	}

	void Log::logMsg(const String& message, LogLevel level)
	{
		LogEntry* newEntry = new LogEntry(message, level);
		mEntries.push_back(newEntry);

		doOnEntryAdded(*newEntry);

		if(mAutoSave)
			save();
	}

	void Log::clear()
	{
		for(auto iter = mEntries.begin(); iter != mEntries.end(); ++iter)
			delete *iter;

		mEntries.clear();

		if(mAutoSave)
			save();
	}

	void Log::save()
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

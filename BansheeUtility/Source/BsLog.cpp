//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsLog.h"
#include "BsException.h"

namespace BansheeEngine
{
	LogEntry::LogEntry(const String& msg, const String& level)
		:mMsg(msg), mChannel(level)
	{ }

	Log::Log()
	{
	}

	Log::~Log()
	{
		BS_LOCK_RECURSIVE_MUTEX(mMutex);

		for(auto iter = mEntries.begin(); iter != mEntries.end(); ++iter)
			bs_delete<PoolAlloc>(*iter);
	}

	void Log::logMsg(const String& message, const String& level)
	{
		BS_LOCK_RECURSIVE_MUTEX(mMutex);

		LogEntry* newEntry = bs_new<LogEntry, PoolAlloc>(message, level);
		mEntries.push_back(newEntry);

		doOnEntryAdded(*newEntry);
	}

	void Log::clear()
	{
		BS_LOCK_RECURSIVE_MUTEX(mMutex);

		for(auto iter = mEntries.begin(); iter != mEntries.end(); ++iter)
			bs_delete<PoolAlloc>(*iter);

		mEntries.clear();
	}

	void Log::saveToFile(const WString& path)
	{
		// TODO - Save the log as HTML
		BS_EXCEPT(NotImplementedException, "Log save to file not yet implemented.");
	}

	void Log::doOnEntryAdded(const LogEntry& entry)
	{
		onEntryAdded(entry);
	}
}
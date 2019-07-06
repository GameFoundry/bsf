//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Debug/BsLog.h"
#include "Error/BsException.h"

namespace bs
{
	Log::~Log()
	{
		clear();
	}

	void Log::logMsg(const String& message, LogVerbosity verbosity, UINT32 category)
	{
		RecursiveLock lock(mMutex);

		mUnreadEntries.push(LogEntry(message, verbosity, category));
	}

	void Log::clear()
	{
		RecursiveLock lock(mMutex);

		mEntries.clear();

		while (!mUnreadEntries.empty())
			mUnreadEntries.pop();

		mHash++;
	}

	void Log::clear(LogVerbosity verbosity, UINT32 category)
	{
		RecursiveLock lock(mMutex);

		Vector<LogEntry> newEntries;
		for(auto& entry : mEntries)
		{
			if (((verbosity == LogVerbosity::Any) || entry.getVerbosity() == verbosity) &&
				(category == (UINT32)-1 || entry.getCategory() == category))
				continue;

			newEntries.push_back(entry);
		}

		mEntries = newEntries;

		Queue<LogEntry> newUnreadEntries;
		while (!mUnreadEntries.empty())
		{
			LogEntry entry = mUnreadEntries.front();
			mUnreadEntries.pop();

			if (((verbosity == LogVerbosity::Any) || entry.getVerbosity() == verbosity) &&
				(category == (UINT32)-1 || entry.getCategory() == category))
				continue;

			newUnreadEntries.push(entry);
		}

		mUnreadEntries = newUnreadEntries;
		mHash++;
	}

	bool Log::getUnreadEntry(LogEntry& entry)
	{
		RecursiveLock lock(mMutex);

		if (mUnreadEntries.empty())
			return false;

		entry = mUnreadEntries.front();
		mUnreadEntries.pop();
		mEntries.push_back(entry);
		mHash++;

		return true;
	}

	bool Log::getLastEntry(LogEntry& entry)
	{
		if (mEntries.size() == 0)
			return false;

		entry = mEntries.back();
		return true;
	}

	Vector<LogEntry> Log::getEntries() const
	{
		RecursiveLock lock(mMutex);

		return mEntries;
	}
	
	bool Log::registerCategory(UINT32 id, String name)
	{
		RecursiveLock lock(mMutex);
		
		if (!categoryExist (id))
		{
			mIdStringPairs.emplace(id, name);
			mNumCategories++;
			return true;
		}
		return false;
	}
	
	void Log::unregisterCategory(UINT32 id)
	{
		RecursiveLock lock(mMutex);
		
		if (categoryExist (id))
		{
			mIdStringPairs.erase(id);
			mNumCategories--;
		}
	}
	
	bool Log::categoryExist(UINT32 id) const
	{
		return mIdStringPairs.find(id) != mIdStringPairs.end();
	}
	
	bool Log::getCategoryName(UINT32 id, String& name) const
	{
		RecursiveLock lock(mMutex);
		
		if (categoryExist(id))
		{
			auto search = mIdStringPairs.find(id);
			name = search->second;
			return true;
		}
		else
		{
			name = "Unknown";
			return false;
		}
	}
	
	Vector<LogEntry> Log::getAllEntries() const
	{
		Vector<LogEntry> entries;
		{
			RecursiveLock lock(mMutex);

			for (auto& entry : mEntries)
				entries.push_back(entry);

			Queue<LogEntry> unreadEntries = mUnreadEntries;
			while (!unreadEntries.empty())
			{
				entries.push_back(unreadEntries.front());
				unreadEntries.pop();
			}
		}
		return entries;
	}
}

#include "BsLog.h"
#include "BsException.h"

namespace BansheeEngine
{
	LogEntry::LogEntry(const String& msg, UINT32 channel)
		:mMsg(msg), mChannel(channel)
	{ }

	Log::Log()
	{
	}

	Log::~Log()
	{
		clear();
	}

	void Log::logMsg(const String& message, UINT32 channel)
	{
		BS_LOCK_RECURSIVE_MUTEX(mMutex);

		mUnreadEntries.push(LogEntry(message, channel));
	}

	void Log::clear()
	{
		BS_LOCK_RECURSIVE_MUTEX(mMutex);

		mEntries.clear();

		while (!mUnreadEntries.empty())
			mUnreadEntries.pop();
	}

	bool Log::getUnreadEntry(LogEntry& entry)
	{
		BS_LOCK_RECURSIVE_MUTEX(mMutex);

		if (mUnreadEntries.empty())
			return false;

		entry = mUnreadEntries.front();
		mUnreadEntries.pop();
		mEntries.push_back(entry);

		return true;
	}

	Vector<LogEntry> Log::getEntries() const
	{
		BS_LOCK_RECURSIVE_MUTEX(mMutex);

		return mEntries;
	}

	Vector<LogEntry> Log::getAllEntries() const
	{
		Vector<LogEntry> entries;
		{
			BS_LOCK_RECURSIVE_MUTEX(mMutex);

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
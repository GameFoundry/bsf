#pragma once

#include "BsPrerequisitesUtil.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	/**
	 * @brief	A single log entry, containing a message and a channel the message
	 * 			was recorded on.
	 */
	class BS_UTILITY_EXPORT LogEntry
	{
	public:
		LogEntry() { }
		LogEntry(const String& msg, UINT32 channel);

		UINT32 getChannel() const { return mChannel; }
		const String& getMessage() const { return mMsg; }

	private:
		String mMsg;
		UINT32 mChannel;
	};

	/**
	 * @brief	Used for logging messages. Can categorize messages according to channels, save the log to a file
	 * 			and send out callbacks when a new message is added.
	 * 			
	 * @note	Thread safe.
	 */
	class BS_UTILITY_EXPORT Log
	{
	public:
		Log();
		~Log();

		/**
		 * @brief	Logs a new message. 
		 *
		 * @param	message	The message describing the log entry.
		 * @param	channel Channel in which to store the log entry.
		 */
		void logMsg(const String& message, UINT32 channel);

		/**
		 * @brief	Removes all log entries. 
		 */
		void clear();

		/**
		 * @brief	Removes all log entries in a specific channel. 
		 */
		void clear(UINT32 channel);

		/**
		 * @brief	Returns all existing log entries.
		 */
		Vector<LogEntry> getEntries() const;

		/**
		 * @brief	Returns the latest unread entry from the log queue, and removes the entry from the unread entries
		 * 			list.
		 * 			
		 * @param	entry	Entry that was retrieved, or undefined if no entries exist.
		 * 					
		 * @returns	True if an unread entry was retrieved, false otherwise.
		 */
		bool getUnreadEntry(LogEntry& entry);

		/**
		 * @brief	Returns the last available log entry.
		 *
		 * @param	entry	Entry that was retrieved, or undefined if no entries exist.
		 *
		 * @returns	True if an entry was retrieved, false otherwise.
		 */
		bool getLastEntry(LogEntry& entry);

		/**
		 * @brief	Returns a hash value that is modified whenever entries in the log change. This can be used for
		 *			checking for changes by external systems.
		 */
		UINT64 getHash() const { return mHash; }

	private:
		friend class Debug;

		/**
		 * @brief	Returns all log entries, including those marked as unread.
		 */
		Vector<LogEntry> getAllEntries() const;

		Vector<LogEntry> mEntries;
		Queue<LogEntry> mUnreadEntries;
		UINT64 mHash;
		BS_RECURSIVE_MUTEX(mMutex);
	};
}
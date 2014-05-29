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
		LogEntry(const String& msg, const String& channel);

		const String& getChannel(void) { return mChannel; }
		const String& getMessage(void) { return mMsg; }

	private:
		String mMsg;
		String mChannel;
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
		void logMsg(const String& message, const String& channel);

		/**
		 * @brief	Removes all log entries. 
		 */
		void clear();

		/**
		 * @brief	Saves the log file to disk.
		 */
		void saveToFile(const WString& path);

	private:
		Vector<LogEntry*> mEntries;
		BS_RECURSIVE_MUTEX(mMutex);

		/**
		 * @brief	Called whenever a new entry is added.
		 */
		void doOnEntryAdded(const LogEntry& entry);

		/************************************************************************/
		/* 								SIGNALS		                     		*/
		/************************************************************************/
	public:
		/**
		 * @brief	Triggered when a new entry in the log is added.
		 * 			
		 * @note	
		 */
		Event<void(const LogEntry&)> onEntryAdded;
	};
}
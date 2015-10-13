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

	private:
		friend class Debug;

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
#pragma once

#include "CmPrerequisitesUtil.h"
#include "boost/signal.hpp"

namespace CamelotFramework
{
	/**
	 * @brief	A single log entry, containing a message and a channel the message
	 * 			was recorded on.
	 */
	class CM_UTILITY_EXPORT LogEntry
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
	 */
	class CM_UTILITY_EXPORT Log
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
		Vector<LogEntry*>::type mEntries;

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
		 */
		boost::signal<void(const LogEntry&)> onEntryAdded;
	};
}
#pragma once

#include "CmPrerequisitesUtil.h"
#include "boost/signal.hpp"

namespace CamelotFramework
{
	/**
	 * @brief	A single log entry, usually used in QtLogModel as
	 * 			a representation of a log entry in the Console window.
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
	 * @brief	Used for logging messages to a file. Can also send out callbacks to
	 * 			registered classes when a message is received, so they can do with it as they wish.
	 */
	class CM_UTILITY_EXPORT Log
	{
	public:
		/**
		 * @brief	Constructor.
		 *
		 * @param	logFilePath		  	Full pathname of the log file. Should have a .html extension.
		 * @param	autoSave		  	(optional) Whether to save the log to disk whenever a new message
		 * 								is added. (Useful when you want to make sure your log is up to
		 * 								date after a crash). The log file will only be saved on exit, or
		 * 								if you call save manually.
		 * @param	suppressFileOutput	(optional) If true, log won't be saved to a file, even if you
		 * 								call save manually.
		 */
		Log(const String& logFilePath = "", bool autoSave = true, bool suppressFileOutput = false);
		~Log();

		/**
		 * @brief	Logs a new message. If autoSave is enabled it will automatically save the message to
		 * 			disk.
		 *
		 * @param	message	The message describing the log entry.
		 * @param	channel Channel in which to store the log entry.
		 */
		void logMsg(const String& message, const String& channel);

		/**
		 * @brief	Removes all log entries. If autoSave is enabled the file on disk will be cleared too.
		 */
		void clear();

		/**
		 * @brief	Saves the log file to disk.
		 */
		void flush();

	private:
		vector<LogEntry*>::type mEntries;

		bool mAutoSave;
		bool mSuppressFileOutput;
		String mSaveFilePath;

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
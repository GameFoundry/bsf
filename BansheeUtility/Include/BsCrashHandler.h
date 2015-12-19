#pragma once

#define BS_MAX_STACKTRACE_DEPTH 200
#define BS_MAX_STACKTRACE_NAME_BYTES 1024

namespace BansheeEngine
{
	/**
	 * @brief	Saves crash data and notifies the user when a crash occurs.
	 */
	// TODO - Crashes are reported in the same process as the main application. This can be a problem if the crash was caused
	// by heap. Any further use of the heap by the reporting methods will cause a silent crash, failing to log it. A more appropriate
	// way of doing it should be to resume another process to actually handle the crash.
	class BS_UTILITY_EXPORT CrashHandler
	{
	public:
		CrashHandler();
		~CrashHandler();

		/**
		 * @brief	Constructs and starts the module.
		 */
		static void startUp() { _instance() = bs_new<CrashHandler>(); }

		/**
		 * @brief	Shuts down this module and frees any resources it is using.
		 */
		static void shutDown() { bs_delete(_instance()); }

		/**
		 * @brief	Returns a reference to the module instance.
		 */
		static CrashHandler& instance() { return *_instance(); }

		/**
		 * @brief	Records a crash with a custom error message.
		 * 			
		 * @param	type		Type of the crash that occurred. e.g. "InvalidParameter".
		 * @param	description	More detailed description of the issue that caused the crash.
		 * @param	function	Optional name of the function where the error occurred.
		 * @param	file		Optional name of the source code file in which the code that crashed the program exists.
		 * @param	line		Optional source code line at which the crash was triggered at.
		 */
		void reportCrash(const String& type, const String& description, const String& function = StringUtil::BLANK,
			const String& file = StringUtil::BLANK, UINT32 line = 0) const;

#if BS_PLATFORM == BS_PLATFORM_WIN32
		/**
		 * @brief	Records a crash resulting from a Windows-specific SEH exception. 
		 * 			
		 * @param	exceptionData	Exception data returned from GetExceptionInformation()
		 * 							
		 * @returns	Code that signals the __except exception handler on how to proceed.
		 */
		int reportCrash(void* exceptionData) const;
#endif

		/**
		 * @brief	Returns a string containing a current stack trace. If function can be found in the symbol
		 * 			table its readable name will be present in the stack trace, otherwise just its address.
		 * 						
		 * @returns	String containing the call stack with each function on its own line.
		 */
		static String getStackTrace();
	private:
		/**
		 * @brief	Returns path to the folder into which to store the crash reports.
		 */
		Path getCrashFolder() const;

		/**
		 * @brief	Returns a singleton instance of this module. 
		 */
		static CrashHandler*& _instance() { static CrashHandler* inst = nullptr; return inst; }

		static const wchar_t* CrashReportFolder;
		static const wchar_t* CrashLogName;

		struct Data;
		Data* m;
	};

	/**
	 * @brief	Returns an instance of the CrashHandler.
	 */
	BS_UTILITY_EXPORT CrashHandler& gCrashHandler();
}
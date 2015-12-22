#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for Debug.
	 */
	class BS_SCR_BE_EXPORT ScriptDebug : public ScriptObject<ScriptDebug>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Debug")

		/**
		 * @brief	Registers internal callbacks. Must be called on scripting system load.
		 */
		static void startUp();

		/**
		 * @brief	Unregisters internal callbacks. Must be called on scripting system shutdown.
		 */
		static void shutDown();
	private:
		ScriptDebug(MonoObject* instance);

		/**
		 * @brief	Triggered when a new entry is added to the debug log.
		 */
		static void onLogEntryAdded(const LogEntry& entry);

		static HEvent mOnLogEntryAddedConn;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_log(MonoString* message);
		static void internal_logWarning(MonoString* message);
		static void internal_logError(MonoString* message);
		static void internal_logMessage(MonoString* message, UINT32 type);
		static void internal_clear();
		static void internal_clearType(UINT32 type);
		static MonoArray* internal_getMessages();

		typedef void(__stdcall *OnAddedThunkDef) (UINT32, MonoString*, MonoException**);

		static OnAddedThunkDef onAddedThunk;
	};
}
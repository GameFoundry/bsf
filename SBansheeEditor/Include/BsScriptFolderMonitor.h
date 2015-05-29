#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"
#include "BsFolderMonitor.h"
#include "BsModule.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptFolderMonitor : public ScriptObject <ScriptFolderMonitor>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "FolderMonitor")

	private:
		friend class ScriptFolderMonitorManager;

		static void internal_CreateInstance(MonoObject* instance, MonoString* folder);
		static void internal_Destroy(ScriptFolderMonitor* thisPtr);

		ScriptFolderMonitor(MonoObject* instance, FolderMonitor* monitor);
		~ScriptFolderMonitor();

		void update();
		void destroy();

		void onMonitorFileModified(const Path& path);
		void onMonitorFileAdded(const Path& path);
		void onMonitorFileRemoved(const Path& path);
		void onMonitorFileRenamed(const Path& from, const Path& to);

		typedef void(__stdcall *OnModifiedThunkDef) (MonoObject*, MonoString*, MonoException**);
		typedef void(__stdcall *OnRenamedThunkDef) (MonoObject*, MonoString*, MonoString*, MonoException**);

		static OnModifiedThunkDef OnModifiedThunk;
		static OnModifiedThunkDef OnAddedThunk;
		static OnModifiedThunkDef OnRemovedThunk;
		static OnRenamedThunkDef OnRenamedThunk;

		FolderMonitor* mMonitor;
	};

	class BS_SCR_BED_EXPORT ScriptFolderMonitorManager : public Module<ScriptFolderMonitorManager>
	{
	public:
		void update();

	private:
		friend class ScriptFolderMonitor;

		void _registerMonitor(ScriptFolderMonitor* monitor);
		void _unregisterMonitor(ScriptFolderMonitor* monitor);

		UnorderedSet<ScriptFolderMonitor*> mMonitors;
	};
}
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptContextMenu : public ScriptObject < ScriptContextMenu >
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ContextMenu")

		GUIContextMenuPtr getInternal() const { return mContextMenu; }

	private:
		static void internal_CreateInstance(MonoObject* instance);
		static void internal_AddItem(ScriptContextMenu* instance, MonoString* path, UINT32 callbackIdx, ShortcutKey shortcut);
		static void internal_AddSeparator(ScriptContextMenu* instance, MonoString* path);
		static void internal_SetLocalizedName(ScriptContextMenu* instance, MonoString* label, ScriptHString* name);

		ScriptContextMenu(MonoObject* instance);

		void onContextMenuItemTriggered(UINT32 idx);

		GUIContextMenuPtr mContextMenu;

		typedef void(__stdcall *OnEntryTriggeredThunkDef) (MonoObject*, UINT32 callbackIdx, MonoException**);

		static OnEntryTriggeredThunkDef onEntryTriggered;
	};
}
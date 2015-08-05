#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for GUIContextMenu.
	 */
	class BS_SCR_BE_EXPORT ScriptContextMenu : public ScriptObject < ScriptContextMenu >
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ContextMenu")

		/**
		 * @brief	Returns the internal native context menu object.
		 */
		GUIContextMenuPtr getInternal() const { return mContextMenu; }

	private:
		ScriptContextMenu(MonoObject* instance);

		/**
		 * @brief	Triggered when an item in the context menu is clicked.
		 *
		 * @param	idx	Sequential index of the item that was clicked.
		 */
		void onContextMenuItemTriggered(UINT32 idx);

		GUIContextMenuPtr mContextMenu;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		typedef void(__stdcall *OnEntryTriggeredThunkDef) (MonoObject*, UINT32 callbackIdx, MonoException**);
		static OnEntryTriggeredThunkDef onEntryTriggered;

		static void internal_CreateInstance(MonoObject* instance);
		static void internal_AddItem(ScriptContextMenu* instance, MonoString* path, UINT32 callbackIdx, ShortcutKey shortcut);
		static void internal_AddSeparator(ScriptContextMenu* instance, MonoString* path);
		static void internal_SetLocalizedName(ScriptContextMenu* instance, MonoString* label, ScriptHString* name);
	};
}
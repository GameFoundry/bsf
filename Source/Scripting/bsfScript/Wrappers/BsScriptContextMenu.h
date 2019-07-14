//********************************* bs::framework - Copyright 2018-2019 Marko Pintera ************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace bs
{
	/** @addtogroup ScriptInteropEngine
	 *  @{
	 */

	/**	Interop class between C++ & CLR for GUIContextMenu. */
	class BS_SCR_BE_EXPORT ScriptContextMenu : public ScriptObject < ScriptContextMenu >
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, ENGINE_NS, "ContextMenu")

		/**	Returns the internal native context menu object. */
		SPtr<GUIContextMenu> getInternal() const { return mContextMenu; }

	private:
		ScriptContextMenu(MonoObject* instance);

		/**
		 * Triggered when an item in the context menu is clicked.
		 *
		 * @param[in]	idx		Sequential index of the item that was clicked.
		 */
		void onContextMenuItemTriggered(UINT32 idx);

		SPtr<GUIContextMenu> mContextMenu;
		UINT32 mGCHandle = 0;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		typedef void(BS_THUNKCALL *OnEntryTriggeredThunkDef) (MonoObject*, UINT32 callbackIdx, MonoException**);
		static OnEntryTriggeredThunkDef onEntryTriggered;

		static void internal_CreateInstance(MonoObject* instance);
		static void internal_Open(ScriptContextMenu* instance, Vector2I* position, ScriptGUILayout* layoutPtr);
		static void internal_AddItem(ScriptContextMenu* instance, MonoString* path, UINT32 callbackIdx, ShortcutKey* shortcut);
		static void internal_AddSeparator(ScriptContextMenu* instance, MonoString* path);
		static void internal_SetLocalizedName(ScriptContextMenu* instance, MonoString* label, ScriptHString* name);
	};

	/** @} */
}

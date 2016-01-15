#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	/**	Interop class between C++ & CLR for a global always-accessible GUIWidget. */
	class BS_SCR_BE_EXPORT ScriptGUI : public ScriptObject <ScriptGUI>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "GUI")

		/**	Creates the globally accessible GUIWidget and makes the system ready to use. */
		static void startUp();

		/** Updates the main camera the widget renders to, if it changes. Should be called every frame. */
		static void update();

		/** Destroys the globally accessible GUIWidget and all its GUI elements. */
		static void shutDown();

	private:
		ScriptGUI(MonoObject* managedInstance);
		~ScriptGUI();

		static SPtr<GUIWidget> sGUIWidget;
		static ScriptGUILayout* sPanel;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_SetSkin(ScriptGUISkin* skin);
		static MonoObject* internal_GetPanel();
	};
}
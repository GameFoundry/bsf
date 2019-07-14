//********************************* bs::framework - Copyright 2018-2019 Marko Pintera ************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "Wrappers/GUI/BsScriptGUIElement.h"
#include "GUI/BsGUITexture.h"

namespace bs
{
	/** @addtogroup ScriptInteropEngine
	 *  @{
	 */

	/**	Interop class between C++ & CLR for GUIInputBox. */
	class BS_SCR_BE_EXPORT ScriptGUIInputBox : public TScriptGUIElement<ScriptGUIInputBox>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, ENGINE_NS, "GUITextBox")

	private:
		ScriptGUIInputBox(MonoObject* instance, GUIInputBox* inputBox);

		/**	Triggered when the value in the native input box changes. */
		void onChanged(const String& newValue);

		/**	Triggered when the user confirms input in the native input box. */
		void onConfirmed();

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_createInstance(MonoObject* instance, bool multiline, MonoString* style, MonoArray* guiOptions);
		static void internal_setText(ScriptGUIInputBox* nativeInstance, MonoString* text);
		static void internal_getText(ScriptGUIInputBox* nativeInstance, MonoString** text);
		static void internal_setTint(ScriptGUIInputBox* nativeInstance, Color* color);

		typedef void(BS_THUNKCALL *OnChangedThunkDef) (MonoObject*, MonoString*, MonoException**);
		typedef void(BS_THUNKCALL *OnConfirmedThunkDef) (MonoObject*, MonoException**);

		static OnChangedThunkDef onChangedThunk;
		static OnConfirmedThunkDef onConfirmedThunk;
	};

	/** @} */
}

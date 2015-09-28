#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for GUIResourceField.
	 */
	class BS_SCR_BED_EXPORT ScriptGUIResourceField : public TScriptGUIElement<ScriptGUIResourceField>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "GUIResourceField")

	private:
		/**
		 * @brief	Triggered when the value in the native resource field changes.
		 *
		 * @param	instance	Managed GUIResourceField instance.
		 * @param	newValue	UUID of the newly selected resource.
		 */
		static void onChanged(MonoObject* instance, const String& newUUID);

		/**
		 * @brief	Retrieves a managed instance of the specified native resource.
		 *			Will return null if one doesn't exist.
		 */
		static MonoObject* nativeToManagedResource(const HResource& instance);

		ScriptGUIResourceField(MonoObject* instance, GUIResourceField* resourceField);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_createInstance(MonoObject* instance, MonoReflectionType* type, MonoObject* title, UINT32 titleWidth,
			MonoString* style, MonoArray* guiOptions, bool withTitle);

		static void internal_getValue(ScriptGUIResourceField* nativeInstance, MonoObject** output);
		static void internal_setValue(ScriptGUIResourceField* nativeInstance, MonoObject* value);
		static void internal_setTint(ScriptGUIResourceField* nativeInstance, Color color);

		typedef void(__stdcall *OnChangedThunkDef) (MonoObject*, MonoObject*, MonoException**);

		static OnChangedThunkDef onChangedThunk;
	};
}
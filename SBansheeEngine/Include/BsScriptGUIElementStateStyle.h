#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsMonoClass.h"
#include "BsGUIElementStyle.h"
#include "BsScriptSpriteTexture.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for GUIElementStateStyle.
	 */
	class BS_SCR_BE_EXPORT ScriptGUIElementStateStyle : public ScriptObject<ScriptGUIElementStateStyle>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "GUIElementStateStyle")

		~ScriptGUIElementStateStyle();

		/**
		 * @brief	Returns a copy of the internal GUIElementStateStyle.
		 */
		GUIElementStyle::GUIElementStateStyle getInternalValue() const { return *mElementStateStyle; }

	private:
		/**
		 * @brief	Creates the interop object with a brand new default style.
		 */
		ScriptGUIElementStateStyle(MonoObject* instance);

		/**
		 * @brief	Creates the interop object by referencing an existing style instance.
		 */
		ScriptGUIElementStateStyle(MonoObject* instance, GUIElementStyle::GUIElementStateStyle* externalStyle);

		GUIElementStyle::GUIElementStateStyle* mElementStateStyle;
		ScriptSpriteTexture* mSpriteTexture;
		bool mOwnsStyle;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_createInstance(MonoObject* instance);
		static void internal_createInstanceExternal(MonoObject* instance, GUIElementStyle::GUIElementStateStyle* externalStateStyle);

		static void internal_GetTexture(ScriptGUIElementStateStyle* nativeInstance, MonoObject** value);
		static void internal_SetTexture(ScriptGUIElementStateStyle* nativeInstance, MonoObject* value);

		static void internal_GetTextColor(ScriptGUIElementStateStyle* nativeInstance, Color* value);
		static void internal_SetTextColor(ScriptGUIElementStateStyle* nativeInstance, Color* value);
	};
}
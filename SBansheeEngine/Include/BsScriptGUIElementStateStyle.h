#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsGUIElementStyle.h"
#include "BsScriptSpriteTexture.h"

namespace BansheeEngine
{
	/**
	 * @brief	Performs conversion between managed GUIElementStateStyle and native GUIElementStyle::GUIElementStateStyle.
	 */
	class BS_SCR_BE_EXPORT ScriptGUIElementStateStyle : public ScriptObject<ScriptGUIElementStateStyle>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "GUIElementStateStyle")

		/**
		 * @brief	Creates a new managed instance of GUIElementStateStyle.
		 *
		 * @param	state	Native GUI element style state to copy to the managed instance.
		 */
		static MonoObject* toManaged(const GUIElementStyle::GUIElementStateStyle& state);

		/**
		 * @brief	Converts a managed instance of GUIElementStateStyle to a native GUI element style state.
		 */
		static GUIElementStyle::GUIElementStateStyle toNative(MonoObject* instance);

	private:
		ScriptGUIElementStateStyle(MonoObject* instance);

		static MonoField* sTextureField;
		static MonoField* sTextColorField;
	};
}
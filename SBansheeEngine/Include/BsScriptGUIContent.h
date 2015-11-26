#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsGUIContent.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for GUIContent.
	 */
	class BS_SCR_BE_EXPORT ScriptGUIContent : public ScriptObject<ScriptGUIContent>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "GUIContent")

		/**
		 * @brief	Retrieves the title text in a managed GUIContent instance.
		 */
		static const HString& getText(MonoObject* instance);

		/**
		 * @brief	Retrieves the tooltip text in a managed GUIContent instance.
		 */
		static const HString& getTooltip(MonoObject* instance);

		/**
		 * @brief	Retrieves the content image in a managed GUIContent instance.
		 */
		static GUIContentImages getImage(MonoObject* instance);

	private:
		ScriptGUIContent(MonoObject* instance);

		static MonoField* mTextField;
		static MonoField* mTooltipField;
		static MonoField* mImagesField;
	};
}
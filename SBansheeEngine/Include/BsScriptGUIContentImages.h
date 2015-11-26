#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsGUIContent.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for GUIContentImages.
	 */
	class BS_SCR_BE_EXPORT ScriptGUIContentImages : public ScriptObject<ScriptGUIContentImages>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "GUIContentImages")

		/**
		 * @brief	Converts a managed GUIContentImages instance to a native one.
		 */
		static GUIContentImages getNative(MonoObject* instance);

		/**
		 * @brief	Converts a native GUIContentImages instance to a managed one.
		 */
		static MonoObject* getManaged(const GUIContentImages& images);

	private:
		ScriptGUIContentImages(MonoObject* instance);

		static MonoField* mNormalField;
		static MonoField* mHoverField;
		static MonoField* mActiveField;
		static MonoField* mFocusedField;
		static MonoField* mNormalOnField;
		static MonoField* mHoverOnField;
		static MonoField* mActiveOnField;
		static MonoField* mFocusedOnField;
	};
}
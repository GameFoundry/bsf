#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIFlexibleSpace : public TScriptGUIElementBase<ScriptGUIFlexibleSpace>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "GUIFlexibleSpace")

	private:
		static void internal_createInstance(MonoObject* instance);

		ScriptGUIFlexibleSpace(MonoObject* instance, GUIFlexibleSpace* flexibleSpace);

		void destroy();

		GUIFlexibleSpace* mFlexibleSpace;
		bool mIsDestroyed;
	};
}
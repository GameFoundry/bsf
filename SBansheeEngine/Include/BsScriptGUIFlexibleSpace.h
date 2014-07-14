#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIFlexibleSpace : public TScriptGUIElementBase<ScriptGUIFlexibleSpace>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "GUIFlexibleSpace")

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* parentLayout);

		ScriptGUIFlexibleSpace(MonoObject* instance, GUIFlexibleSpace& flexibleSpace, GUILayout* parentLayout);

		void destroy();
		void setParent(GUILayout* parentLayout);

		GUIFlexibleSpace& mFlexibleSpace;
		GUILayout* mParentLayout;
		bool mIsDestroyed;
	};
}
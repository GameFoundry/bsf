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
		static void internal_createInstanceAdd(MonoObject* instance, MonoObject* parentLayout);
		static void internal_createInstanceInsert(MonoObject* instance, MonoObject* parentLayout, UINT32 index);

		ScriptGUIFlexibleSpace(MonoObject* instance, GUIFlexibleSpace& flexibleSpace, GUILayout* parentLayout);

		void destroy();

		GUIFlexibleSpace& mFlexibleSpace;
		GUILayout* mParentLayout;
		bool mIsDestroyed;
	};
}
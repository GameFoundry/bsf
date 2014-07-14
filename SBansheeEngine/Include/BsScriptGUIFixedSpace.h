#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIFixedSpace : public TScriptGUIElementBase<ScriptGUIFixedSpace>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "GUIFixedSpace")

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* parentLayout, UINT32 size);
		static void internal_setSize(ScriptGUIFixedSpace* nativeInstance, UINT32 size);

		ScriptGUIFixedSpace(MonoObject* instance, GUIFixedSpace& fixedSpace, GUILayout* parentLayout);

		void destroy();
		void setParent(GUILayout* parentLayout);

		GUIFixedSpace& mFixedSpace;
		GUILayout* mParentLayout;
		bool mIsDestroyed;
	};
}
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIFixedSpace : public ScriptObject<ScriptGUIFixedSpace>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "GUIFixedSpace")

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* parentLayout, UINT32 size);

		static void internal_destroy(ScriptGUIFixedSpace* nativeInstance);
		static void internal_setVisible(ScriptGUIFixedSpace* nativeInstance, bool visible);
		static void internal_setParent(ScriptGUIFixedSpace* nativeInstance, MonoObject* parentLayout);

		ScriptGUIFixedSpace(MonoObject* instance, GUIFixedSpace& fixedSpace, GUILayout* parentLayout);

		void destroy();
		void _onManagedInstanceDeleted();

		GUIFixedSpace& mFixedSpace;
		GUILayout* mParentLayout;
		bool mIsDestroyed;
	};
}
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIFlexibleSpace : public ScriptObject<ScriptGUIFlexibleSpace>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "GUIFlexibleSpace")

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* parentLayout);

		static void internal_destroy(ScriptGUIFlexibleSpace* nativeInstance);
		static void internal_setVisible(ScriptGUIFlexibleSpace* nativeInstance, bool visible);
		static void internal_setParent(ScriptGUIFlexibleSpace* nativeInstance, MonoObject* parentLayout);

		ScriptGUIFlexibleSpace(MonoObject* instance, GUIFlexibleSpace& flexibleSpace, GUILayout* parentLayout);

		void destroy();
		void _onManagedInstanceDeleted();

		GUIFlexibleSpace& mFlexibleSpace;
		GUILayout* mParentLayout;
		bool mIsDestroyed;
	};
}
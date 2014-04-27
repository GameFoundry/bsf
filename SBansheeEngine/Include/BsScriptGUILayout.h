#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUILayout : public ScriptObject<ScriptGUILayout>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "GUILayout")

		GUILayout* getInternalValue() const { return mLayout; }
		void* getNativeRaw() const { return mLayout; }

	private:
		static void internal_createInstanceXFromArea(MonoObject* instance, MonoObject* parentArea);
		static void internal_createInstanceXFromLayout(MonoObject* instance, MonoObject* parentLayout);
		static void internal_createInstanceYFromLayout(MonoObject* instance, MonoObject* parentLayout);
		static void internal_createInstanceYFromScrollArea(MonoObject* instance, MonoObject* parentScrollArea);

		static void internal_destroy(ScriptGUILayout* nativeInstance);
		static void internal_setVisible(ScriptGUILayout* nativeInstance, bool visible);
		static void internal_setParent(ScriptGUILayout* nativeInstance, MonoObject* parentLayout);

		ScriptGUILayout(MonoObject* instance, GUILayout* layout, GUILayout* parentLayout);

		void destroy();
		void _onManagedInstanceDeleted();

		GUILayout* mLayout;
		GUILayout* mParentLayout;
		bool mIsDestroyed;
	};
}
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUILayout : public TScriptGUIElementBase<ScriptGUILayout>
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

		ScriptGUILayout(MonoObject* instance, GUILayout* layout, GUILayout* parentLayout);

		void destroy();
		void setParent(GUILayout* parentLayout);

		GUILayout* mLayout;
		GUILayout* mParentLayout;
		bool mIsDestroyed;
	};
}
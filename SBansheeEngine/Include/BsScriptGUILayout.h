#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUILayout : public ScriptObject<ScriptGUILayout>
	{
	public:
		static void initMetaData();

		GUILayout* getInternalValue() const { return mLayout; }
		void* getNativeRaw() const { return mLayout; }

		GUIWidget& getParentWidget() const { return mParentWidget; }

	private:
		static void internal_createInstanceXFromArea(MonoObject* instance, MonoObject* parentArea);
		static void internal_createInstanceXFromLayout(MonoObject* instance, MonoObject* parentLayout);
		static void internal_createInstanceYFromLayout(MonoObject* instance, MonoObject* parentLayout);
		static void internal_createInstanceYFromScrollArea(MonoObject* instance, MonoObject* parentScrollArea);
		static void internal_destroyInstance(ScriptGUILayout* nativeInstance);

		static void internal_destroy(ScriptGUILayout* nativeInstance);
		static void internal_disable(ScriptGUILayout* nativeInstance);
		static void internal_enable(ScriptGUILayout* nativeInstance);

		static void initRuntimeData();

		ScriptGUILayout(GUILayout* layout, GUIWidget& parentWidget, GUILayout* parentLayout);

		GUILayout* mLayout;
		GUILayout* mParentLayout;
		GUIWidget& mParentWidget;
	};
}
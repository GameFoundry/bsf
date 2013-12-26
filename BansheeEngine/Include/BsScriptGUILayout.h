#pragma once

#include "BsPrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_EXPORT ScriptGUILayout : public ScriptObject<ScriptGUILayout>
	{
	public:
		static void initMetaData();

		GUILayout* getInternalValue() const { return mLayout; }
		void* getNativeRaw() const { return mLayout; }

		ScriptGUIArea* getParentArea() const { return mParentArea; }

	private:
		static void internal_createInstanceXFromArea(MonoObject* instance, MonoObject* parentArea);
		static void internal_createInstanceXFromLayout(MonoObject* instance, MonoObject* parentLayout);
		static void internal_createInstanceYFromLayout(MonoObject* instance, MonoObject* parentLayout);
		static void internal_destroyInstance(ScriptGUILayout* nativeInstance);

		static void initRuntimeData();

		ScriptGUILayout(GUILayout* layout, ScriptGUIArea* parentArea);

		GUILayout* mLayout;
		ScriptGUIArea* mParentArea;
	};
}
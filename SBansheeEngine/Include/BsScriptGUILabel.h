#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUILabel : public ScriptObject<ScriptGUILabel>
	{
	public:
		static void initMetaData();

		GUILabel* getInternalValue() const { return mLabel; }
		void* getNativeRaw() const { return mLabel; }

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* content, MonoString* style, MonoArray* guiOptions);
		static void internal_destroyInstance(ScriptGUILabel* nativeInstance);
		static void internal_setContent(ScriptGUILabel* nativeInstance, MonoObject* content);

		static void internal_destroy(ScriptGUILabel* nativeInstance);
		static void internal_setVisible(ScriptGUILabel* nativeInstance, bool visible);
		static void internal_setParent(ScriptGUILabel* nativeInstance, MonoObject* parentLayout);

		static void initRuntimeData();

		ScriptGUILabel(GUILabel* label);

		void destroy();

		GUILabel* mLabel;
		bool mIsDestroyed;
	};
}
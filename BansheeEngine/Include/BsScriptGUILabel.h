#pragma once

#include "BsPrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_EXPORT ScriptGUILabel : public ScriptObject<ScriptGUILabel>
	{
	public:
		static void initMetaData();

		GUILabel* getInternalValue() const { return mLabel; }
		void* getNativeRaw() const { return mLabel; }

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* parentLayout, MonoString* label, MonoObject* style, MonoArray* guiOptions);
		static void internal_destroyInstance(ScriptGUILabel* nativeInstance);

		static void initRuntimeData();

		ScriptGUILabel(GUILabel* label);

		GUILabel* mLabel;
	};
}
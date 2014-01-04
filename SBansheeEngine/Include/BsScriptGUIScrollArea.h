#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIScrollArea : public ScriptObject<ScriptGUIScrollArea>
	{
	public:
		static void initMetaData();

		GUIScrollArea* getInternalValue() const { return mScrollArea; }
		void* getNativeRaw() const { return mScrollArea; }

		GUIWidget& getParentWidget() const;
	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* parentLayout, MonoObject* style, MonoArray* guiOptions);
		static void internal_destroyInstance(ScriptGUIScrollArea* nativeInstance);

		static void initRuntimeData();

		ScriptGUIScrollArea(GUIScrollArea* button, GUIWidget& parentWidget);

		GUIScrollArea* mScrollArea;
		GUIWidget& mParentWidget;
	};
}
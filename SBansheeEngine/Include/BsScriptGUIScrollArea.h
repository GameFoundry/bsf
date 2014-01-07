#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsGUIScrollArea.h"

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
		static void internal_createInstance(MonoObject* instance, MonoObject* parentLayout, ScrollBarType vertBarType, ScrollBarType horzBarType, 
			MonoObject* scrollBarStyle, MonoObject* scrollAreaStyle, MonoArray* guiOptions);
		static void internal_destroyInstance(ScriptGUIScrollArea* nativeInstance);

		static void internal_destroy(ScriptGUIScrollArea* nativeInstance);
		static void internal_disable(ScriptGUIScrollArea* nativeInstance);
		static void internal_enable(ScriptGUIScrollArea* nativeInstance);

		static void initRuntimeData();

		ScriptGUIScrollArea(GUIScrollArea* scrollArea, GUIWidget& parentWidget);

		GUIScrollArea* mScrollArea;
		GUIWidget& mParentWidget;
	};
}
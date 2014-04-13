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

	private:
		static void internal_createInstance(MonoObject* instance, ScrollBarType vertBarType, ScrollBarType horzBarType, 
			MonoString* scrollBarStyle, MonoString* scrollAreaStyle, MonoArray* guiOptions);
		static void internal_destroyInstance(ScriptGUIScrollArea* nativeInstance);

		static void internal_destroy(ScriptGUIScrollArea* nativeInstance);
		static void internal_setVisible(ScriptGUIScrollArea* nativeInstance, bool visible);
		static void internal_setParent(ScriptGUIScrollArea* nativeInstance, MonoObject* parentLayout);

		static void initRuntimeData();

		ScriptGUIScrollArea(GUIScrollArea* scrollArea);

		void destroy();

		GUIScrollArea* mScrollArea;
		bool mIsDestroyed;
	};
}
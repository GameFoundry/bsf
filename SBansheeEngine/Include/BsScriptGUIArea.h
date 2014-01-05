#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIArea : public ScriptObject<ScriptGUIArea>
	{
	public:
		static void initMetaData();

		GUIArea* getInternalValue() const { return mArea; }
		void* getNativeRaw() const { return mArea; }

		GUIWidget& getParentWidget() const;

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* parentGUI, CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height, CM::UINT16 depth);
		static void internal_createInstanceResizeableX(MonoObject* instance, MonoObject* parentGUI, CM::UINT32 offsetLeft, CM::UINT32 offsetRight, 
			CM::UINT32 offsetTop, CM::UINT32 height, CM::UINT16 depth);

		static void internal_createInstanceResizeableY(MonoObject* instance, MonoObject* parentGUI, CM::UINT32 offsetTop, 
			CM::UINT32 offsetBottom, CM::UINT32 offsetLeft, CM::UINT32 width, CM::UINT16 depth);

		static void internal_createInstanceResizeableXY(MonoObject* instance, MonoObject* parentGUI, CM::UINT32 offsetLeft, 
			CM::UINT32 offsetRight, CM::UINT32 offsetTop, CM::UINT32 offsetBottom, CM::UINT16 depth);

		static void internal_destroyInstance(ScriptGUIArea* nativeInstance);

		static void internal_destroy(ScriptGUIArea* nativeInstance);
		static void internal_disable(ScriptGUIArea* nativeInstance);
		static void internal_enable(ScriptGUIArea* nativeInstance);

		static void initRuntimeData();

		ScriptGUIArea(GUIArea* area, ScriptGUIBase* parentGUI);

		GUIArea* mArea;
		ScriptGUIBase* mParentGUI;
	};
}
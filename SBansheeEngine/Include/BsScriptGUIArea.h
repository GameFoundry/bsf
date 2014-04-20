#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "CmRectI.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIArea : public ScriptObject<ScriptGUIArea>
	{
	public:
		static void initMetaData();

		GUIArea* getInternalValue() const { return mGUIArea; }
		void* getNativeRaw() const { return mGUIArea; }

		void updateArea();

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* parentGUI, CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height, CM::UINT16 depth);
		static void internal_destroyInstance(ScriptGUIArea* nativeInstance);

		static void internal_destroy(ScriptGUIArea* thisPtr);
		static void internal_setVisible(ScriptGUIArea* thisPtr, bool visible);
		static void internal_setArea(ScriptGUIArea* thisPtr, CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height, CM::UINT16 depth);

		static void initRuntimeData();

		ScriptGUIArea(GUIArea* area, ScriptGUIPanel* panel);

		void destroy();

		GUIArea* mGUIArea;
		CM::RectI mArea;
		ScriptGUIPanel* mParentPanel;
		bool mIsDestroyed;
	};
}
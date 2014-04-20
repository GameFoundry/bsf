#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "CmRectI.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIPanel : public ScriptObject<ScriptGUIPanel>
	{
	public:
		static void initMetaData();

		GUIWidget& getWidget() const { return *mParentWidget; }
		const CM::RectI& getClippedArea() const { return mClippedArea; }

		void setParentArea(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height);
		void setParentWidget(GUIWidget* widget);

		void registerArea(ScriptGUIArea* area);
		void unregisterArea(ScriptGUIArea* area);

	protected:
		ScriptGUIPanel();

		static void initRuntimeData();

		static void internal_createInstance(MonoObject* instance);
		static void internal_destroyInstance(ScriptGUIPanel* thisPtr);
		static void internal_setArea(ScriptGUIPanel* thisPtr, CM::INT32 x, CM::INT32 y, 
			CM::UINT32 width, CM::UINT32 height, CM::UINT16 depth);

		void updateArea();

		GUIWidget* mParentWidget;
		CM::Vector<ScriptGUIArea*>::type mAreas;

		CM::RectI mParentArea;
		CM::RectI mMyArea;
		CM::RectI mClippedArea;
	};
}
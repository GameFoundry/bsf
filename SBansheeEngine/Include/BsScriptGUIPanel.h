#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "CmRectI.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIPanel : public ScriptObject<ScriptGUIPanel>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "GUIPanel")

		GUIWidget& getWidget() const { return *mParentWidget; }
		const RectI& getClippedArea() const { return mClippedArea; }

		void setParentArea(INT32 x, INT32 y, UINT32 width, UINT32 height);
		void setParentWidget(GUIWidget* widget);

		void registerArea(ScriptGUIArea* area);
		void unregisterArea(ScriptGUIArea* area);

	protected:
		ScriptGUIPanel(MonoObject* instance);

		static void internal_createInstance(MonoObject* instance);
		static void internal_setArea(ScriptGUIPanel* thisPtr, INT32 x, INT32 y, 
			UINT32 width, UINT32 height, UINT16 depth);

		void updateArea();

		GUIWidget* mParentWidget;
		Vector<ScriptGUIArea*> mAreas;

		RectI mParentArea;
		RectI mMyArea;
		RectI mClippedArea;
	};
}
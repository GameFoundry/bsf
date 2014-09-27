#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsRect2I.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIPanel : public ScriptObject<ScriptGUIPanel>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "GUIPanel")

		GUIWidget& getWidget() const { return *mParentWidget; }
		const Rect2I& getClippedArea() const { return mClippedArea; }

		void setParentArea(INT32 x, INT32 y, UINT32 width, UINT32 height);
		void setParentWidget(GUIWidget* widget);

		void registerArea(ScriptGUIArea* area);
		void unregisterArea(ScriptGUIArea* area);

		const Vector<ScriptGUIArea*>& getAreas() const { return mAreas; }
		Rect2I getArea() const { return mMyArea; }

	protected:
		ScriptGUIPanel(MonoObject* instance);

		static void internal_createInstance(MonoObject* instance);
		static void internal_setArea(ScriptGUIPanel* thisPtr, INT32 x, INT32 y, 
			UINT32 width, UINT32 height, UINT16 depth);

		void updateArea();

		GUIWidget* mParentWidget;
		Vector<ScriptGUIArea*> mAreas;

		Rect2I mParentArea;
		Rect2I mMyArea;
		Rect2I mClippedArea;
	};
}
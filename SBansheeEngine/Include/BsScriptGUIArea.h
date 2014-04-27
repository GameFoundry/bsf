#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "CmRectI.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIArea : public ScriptObject<ScriptGUIArea>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "GUIArea")

		GUIArea* getInternalValue() const { return mGUIArea; }
		void* getNativeRaw() const { return mGUIArea; }

		void updateArea();

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* parentGUI, INT32 x, INT32 y, UINT32 width, UINT32 height, UINT16 depth);

		static void internal_destroy(ScriptGUIArea* thisPtr);
		static void internal_setVisible(ScriptGUIArea* thisPtr, bool visible);
		static void internal_setArea(ScriptGUIArea* thisPtr, INT32 x, INT32 y, UINT32 width, UINT32 height, UINT16 depth);

		ScriptGUIArea(MonoObject* instance, GUIArea* area, ScriptGUIPanel* panel);

		void destroy();
		void _onManagedInstanceDeleted();

		GUIArea* mGUIArea;
		RectI mArea;
		ScriptGUIPanel* mParentPanel;
		bool mIsDestroyed;
	};
}
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIBase : public ScriptObject<ScriptGUIBase>
	{
	public:
		static void initMetaData();

		GUIWidget& getWidget() const { return mWidget; }

	private:
		static void internal_createInstance(MonoObject* instance);
		static void internal_destroyInstance(ScriptGUIBase* nativeInstance);

		static void initRuntimeData();

		ScriptGUIBase(GUIWidget& widget);

		GUIWidget& mWidget;
	};
}
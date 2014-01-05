#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIFixedSpace : public ScriptObject<ScriptGUIFixedSpace>
	{
	public:
		static void initMetaData();

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* parentLayout, CM::UINT32 size);
		static void internal_destroyInstance(ScriptGUIFixedSpace* nativeInstance);

		static void internal_destroy(ScriptGUIFixedSpace* nativeInstance);
		static void internal_disable(ScriptGUIFixedSpace* nativeInstance);
		static void internal_enable(ScriptGUIFixedSpace* nativeInstance);

		static void initRuntimeData();

		ScriptGUIFixedSpace(GUIFixedSpace& fixedSpace, GUILayout* parentLayout);

		GUIFixedSpace& mFixedSpace;
		GUILayout* mParentLayout;

	};
}
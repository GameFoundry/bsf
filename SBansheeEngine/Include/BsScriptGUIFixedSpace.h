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
		static void internal_setVisible(ScriptGUIFixedSpace* nativeInstance, bool visible);
		static void internal_setParent(ScriptGUIFixedSpace* nativeInstance, MonoObject* parentLayout);

		static void initRuntimeData();

		ScriptGUIFixedSpace(GUIFixedSpace& fixedSpace, GUILayout* parentLayout);

		void destroy();

		GUIFixedSpace& mFixedSpace;
		GUILayout* mParentLayout;
		bool mIsDestroyed;
	};
}
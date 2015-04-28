#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIFixedSpace : public TScriptGUIElementBase<ScriptGUIFixedSpace>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "GUIFixedSpace")

	private:
		static void internal_createInstance(MonoObject* instance, UINT32 size);
		static void internal_setSize(ScriptGUIFixedSpace* nativeInstance, UINT32 size);

		ScriptGUIFixedSpace(MonoObject* instance, GUIFixedSpace* fixedSpace);

		void destroy();

		GUIFixedSpace* mFixedSpace;
		bool mIsDestroyed;
	};
}
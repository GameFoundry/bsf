#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIToggleGroup : public ScriptObject<ScriptGUIToggleGroup>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "GUIToggleGroup")

		std::shared_ptr<GUIToggleGroup> getInternalValue() const { return mToggleGroup; }
		void* getNativeRaw() const { return mToggleGroup.get(); }

	private:
		static void internal_createInstance(MonoObject* instance);

		ScriptGUIToggleGroup(MonoObject* instance, const std::shared_ptr<GUIToggleGroup>& toggleGroup);

		std::shared_ptr<GUIToggleGroup> mToggleGroup;
	};
}
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIToggleGroup : public ScriptObject<ScriptGUIToggleGroup>
	{
	public:
		static void initMetaData();

		std::shared_ptr<GUIToggleGroup> getInternalValue() const { return mToggleGroup; }
		void* getNativeRaw() const { return mToggleGroup.get(); }

	private:
		static void internal_createInstance(MonoObject* instance);
		static void internal_destroyInstance(ScriptGUIToggleGroup* nativeInstance);

		static void initRuntimeData();

		ScriptGUIToggleGroup(const std::shared_ptr<GUIToggleGroup>& toggleGroup);

		std::shared_ptr<GUIToggleGroup> mToggleGroup;
	};
}
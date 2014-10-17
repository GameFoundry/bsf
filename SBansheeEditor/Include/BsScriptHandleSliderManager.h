#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptHandleSliderManager : public Module<ScriptHandleSliderManager>
	{
	public:
		~ScriptHandleSliderManager();

		void registerSlider(ScriptHandleSliderBase* slider);
		void unregisterSlider(ScriptHandleSliderBase* slider);

	private:
		Set<ScriptHandleSliderBase*> mSliders;
	};
}
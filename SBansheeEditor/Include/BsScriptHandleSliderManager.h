#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Tracks all managed handle sliders.
	 */
	class BS_SCR_BED_EXPORT ScriptHandleSliderManager : public Module<ScriptHandleSliderManager>
	{
	public:
		~ScriptHandleSliderManager();

		/**
		 * @brief	Registers a new active managed handle slider.
		 */
		void registerSlider(ScriptHandleSliderBase* slider);

		/**
		 * @brief	Unregisters a managed handle slider when it is destroyed.
		 */
		void unregisterSlider(ScriptHandleSliderBase* slider);

	private:
		Set<ScriptHandleSliderBase*> mSliders;
	};
}
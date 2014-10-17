#include "BsScriptHandleSliderManager.h"
#include "BsScriptHandleSlider.h"

namespace BansheeEngine
{
	ScriptHandleSliderManager::~ScriptHandleSliderManager()
	{
		for (auto& slider : mSliders)
			slider->destroyInternal();
	}

	void ScriptHandleSliderManager::registerSlider(ScriptHandleSliderBase* slider)
	{
		mSliders.insert(slider);
	}

	void ScriptHandleSliderManager::unregisterSlider(ScriptHandleSliderBase* slider)
	{
		mSliders.erase(slider);
	}
}
#pragma once

#include "BsEditorPrerequisites.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT HandleSliderManager
	{
	public:
		HandleSliderManager();
		~HandleSliderManager();

		void update(const HCamera& camera);
		void handleInput(const HCamera& camera, const Vector2I& inputPos, const Ray& inputRay, bool pressed);
		bool isSliderActive() const;

		void _registerSlider(HandleSlider* slider);
		void _unregisterSlider(HandleSlider* slider);

	private:
		HandleSlider* mActiveSlider;
		HandleSlider* mHoverSlider;
		UnorderedSet<HandleSlider*> mSliders;
	};
}
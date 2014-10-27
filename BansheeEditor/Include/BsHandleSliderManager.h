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
		bool hasHitSlider(const HCamera& camera, const Vector2I& inputPos) const;
		void handleInput(const HCamera& camera, const Vector2I& inputPos, bool pressed);
		bool isSliderActive() const;

		void _registerSlider(HandleSlider* slider);
		void _unregisterSlider(HandleSlider* slider);

	private:
		HandleSlider* mActiveSlider;
		HandleSlider* mHoverSlider;
		UnorderedSet<HandleSlider*> mSliders;
	};
}
#pragma once

#include "BsEditorPrerequisites.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT HandleSliderManager
	{
	public:
		HandleSliderManager();
		~HandleSliderManager();

		void update(const CameraHandlerPtr& camera, const Vector2I& inputPos, const Vector2I& inputDelta);
		void trySelect(const CameraHandlerPtr& camera, const Vector2I& inputPos);
		void clearSelection();
		bool isSliderActive() const { return mActiveSlider != nullptr; }

		void _registerSlider(HandleSlider* slider);
		void _unregisterSlider(HandleSlider* slider);

	private:
		HandleSlider* findUnderCursor(const CameraHandlerPtr& camera, const Vector2I& inputPos) const;

		HandleSlider* mActiveSlider;
		HandleSlider* mHoverSlider;
		UnorderedSet<HandleSlider*> mSliders;
	};
}
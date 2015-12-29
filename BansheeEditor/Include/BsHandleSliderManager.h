#pragma once

#include "BsEditorPrerequisites.h"

namespace BansheeEngine
{
	/**
	 * @brief	Controls all instantiated HandleSlider%s.
	 */
	class BS_ED_EXPORT HandleSliderManager
	{
	public:
		HandleSliderManager();
		~HandleSliderManager();

		/**
		 * @brief	Updates all underlying sliders, changing their state and dragging them depending
		 *			on their state and pointer movement.
		 *
		 * @param	camera		Camera through which we're interacting with sliders.
		 * @param	inputPos	Position of the pointer.
		 * @param	inputDelta	Movement of the pointer since last frame.
		 */
		void update(const CameraPtr& camera, const Vector2I& inputPos, const Vector2I& inputDelta);

		/**
		 * @brief	Attempts to select (activate) a slider at the specified position (if there is any).
		 *
		 * @param	camera		Camera through which we're interacting with sliders.
		 * @param	inputPos	Position of the pointer.
		 */
		void trySelect(const CameraPtr& camera, const Vector2I& inputPos);

		/**
		 * @brief	Clears the active slider (deactivates it).
		 */
		void clearSelection();

		/**
		 * @brief	Checks is any slider active.
		 */
		bool isSliderActive() const { return mActiveSlider != nullptr; }

		/**
		 * @brief	Registers a new instantiated slider.
		 */
		void _registerSlider(HandleSlider* slider);

		/**
		 * @brief	Unregisters a previously instantiated slider.
		 */
		void _unregisterSlider(HandleSlider* slider);

	private:

		/**
		 * @brief	Attempts to find slider at the specified position.
		 *
		 * @param	camera		Camera through which we're interacting with sliders.
		 * @param	inputPos	Position of the pointer.
		 *
		 * @return	Slider if we're intersecting with one, or null otherwise.
		 */
		HandleSlider* findUnderCursor(const CameraPtr& camera, const Vector2I& inputPos) const;

		HandleSlider* mActiveSlider;
		HandleSlider* mHoverSlider;
		UnorderedSet<HandleSlider*> mSliders;
	};
}
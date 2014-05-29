#pragma once

#include "BsCorePrerequisites.h"
#include "BsInputFwd.h"
#include "BsEvent.h"
#include "BsVector2I.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains relative and absolute position
	 * 			of an input axis. Relative state represents
	 * 			the difference between current and last state.
	 */
	struct RawAxisState
	{
		Vector2I rel;
		Vector2I abs;
	};

	/**
	 * @brief	Different types of input axes.
	 */
	enum class RawInputAxis
	{
		Mouse_XY,
		Mouse_Z,
		Joy_1,
		Joy_2,
		Joy_3,
		Joy_4,
		Joy_5,
		Joy_6,
		Joy_7,
		Joy_8,
		Joy_9,
		Joy_10,
		Joy_11,
		Joy_12,
		Joy_13,
		Joy_14,
		Joy_15,
		Joy_16,
		Count
	};

	/**
	 * @brief	Represents a specific way of acquiring low-level input. InputManager (which provides a higher level input)
	 * 			must have at least one RawInputHandler attached. Raw input handlers receive input as sent by the hardware
	 * 			without OS modifications.
	 * 			
	 *			Attach events handler to the provided signals to handle input.
	 */
	class BS_CORE_EXPORT RawInputHandler
	{
	public:
		RawInputHandler() {}
		virtual ~RawInputHandler() {}

		/**
		 * @brief	Triggered when user presses a button. Parameters
		 * 			include button code of the pressed button, and a timestamp of
		 * 			the button press event.
		 */
		Event<void(ButtonCode, UINT64)> onButtonDown;

		/**
		 * @brief	Triggered when user releases a button. Parameters
		 * 			include button code of the released button, and a timestamp of
		 * 			the button release event.
		 */
		Event<void(ButtonCode, UINT64)> onButtonUp;

		/**
		 * @brief	Triggered whenever the specified axis state changes.
		 */
		Event<void(const RawAxisState&, RawInputAxis)> onAxisMoved;

		/**
		 * @brief	Called once per frame. Capture input here if needed.
		 * 			
		 * @note	Internal method.
		 */
		virtual void _update() {}

		/**
		 * @brief	Called whenever the active window changes.
		 *
		 * @param	win	Newly active window.
		 * 				
		 * @note	Internal method.
		 */
		virtual void _inputWindowChanged(const RenderWindow& win) {}
	};
}
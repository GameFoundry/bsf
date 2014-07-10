//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
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
		RawAxisState()
		 :rel(0.0f), abs(0.0f)
		{ }

		float rel;
		float abs;
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
		RawInputHandler()
			:mMouseSmoothingEnabled(false) 
		{}

		virtual ~RawInputHandler() {}

		/**
		 * @brief	Triggered when user presses a button. Parameters
		 * 			include device index, button code of the pressed button, 
		 *			and a timestamp of the button press event.
		 */
		Event<void(UINT32, ButtonCode, UINT64)> onButtonDown;

		/**
		 * @brief	Triggered when user releases a button. Parameters
		 * 			include device index, button code of the released button, 
		 *			and a timestamp of the button release event.
		 */
		Event<void(UINT32, ButtonCode, UINT64)> onButtonUp;

		/**
		 * @brief	Triggered whenever the specified axis state changes.
		 *			Parameters include device index, axis state data, and axis type.
		 */
		Event<void(UINT32, const RawAxisState&, UINT32)> onAxisMoved;

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

		/**
		 * @brief	Enables or disables mouse smoothing. Smoothing makes the changes to
		 *			mouse axes more gradual.
		 */
		void setMouseSmoothing(bool enabled) { mMouseSmoothingEnabled = enabled; }

	protected:
		bool mMouseSmoothingEnabled;
	};
}
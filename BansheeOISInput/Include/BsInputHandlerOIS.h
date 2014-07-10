//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsOISPrerequisites.h"
#include "BsRawInputHandler.h"

#include <OIS/OISEvents.h>
#include <OIS/OISInputManager.h>
#include <OIS/OISKeyboard.h>
#include <OIS/OISMouse.h>
#include <OIS/OISJoystick.h>

namespace BansheeEngine
{
	class InputHandlerOIS;

	/**
	 * @brief	Listens for events from a specific OIS joystick device.
	 */
	class BS_OIS_EXPORT GamepadEventListener : public OIS::JoyStickListener
	{
	public:
		GamepadEventListener(InputHandlerOIS* parentHandler, UINT32 joystickIdx);

		/**
		 * @brief	Called by OIS whenever a gamepad/joystick button is pressed.
		 */
		virtual bool buttonPressed(const OIS::JoyStickEvent& arg, int button);

		/**
		 * @brief	Called by OIS whenever a gamepad/joystick button is released.
		 */
		virtual bool buttonReleased(const OIS::JoyStickEvent& arg, int button);

		/**
		 * @brief	Called by OIS whenever a gamepad/joystick axis is moved.
		 */
		virtual bool axisMoved(const OIS::JoyStickEvent& arg, int axis);

	private:
		UINT32 mGamepadIdx;
		InputHandlerOIS* mParentHandler;
	};

	/**
	 * @brief	Raw input handler using OIS library for acquiring input.
	 */
	class BS_OIS_EXPORT InputHandlerOIS : public RawInputHandler, public OIS::KeyListener, 
		public OIS::MouseListener
	{
		/**
		 * @brief	Holding data about an active gamepad object.
		 */
		struct GamepadData
		{
			OIS::JoyStick* gamepad;
			GamepadEventListener* listener;
		};

	public:
		InputHandlerOIS(unsigned int hWnd);
		virtual ~InputHandlerOIS();

	private:
		/**
		 * @brief	Called by OIS whenever a keyboard button is pressed.
		 */
		virtual bool keyPressed(const OIS::KeyEvent& arg);

		/**
		 * @brief	Called by OIS whenever a keyboard button is released.
		 */
		virtual bool keyReleased(const OIS::KeyEvent& arg);

		/**
		 * @brief	Called by OIS whenever mouse is moved.
		 */
		virtual bool mouseMoved(const OIS::MouseEvent& arg);

		/**
		 * @brief	Called by OIS whenever is a mouse button pressed.
		 */
		virtual bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id);

		/**
		 * @brief	Called by OIS whenever is a mouse button released
		 */
		virtual bool mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id);

		/**
		 * @brief	Called once per frame.
		 *
		 * @note	Internal method.
		 */
		virtual void _update();

		/**
		 * @brief	Called whenever the currently focused window changes.
		 *
		 * @note	Internal method.
		 */
		virtual void _inputWindowChanged(const RenderWindow& win);

		/**
		 * @brief	Converts an OIS key code into engine button code.
		 */
		static ButtonCode keyCodeToButtonCode(OIS::KeyCode keyCode);

		/**
		 * @brief	Converts an OIS mouse button code into engine button code.
		 */
		static ButtonCode mouseButtonToButtonCode(OIS::MouseButtonID mouseBtn);

		/**
		 * @brief	Converts an OIS gamepad button code into engine button code.
		 */
		static ButtonCode gamepadButtonToButtonCode(INT32 joystickCode);

	private:
		friend class GamepadEventListener;

		/**
		 * @brief	Smooths the input mouse axis value. Smoothing makes the changes to
		 *			the axis more gradual depending on previous values.
		 *
		 * @param	value	Value to smooth.
		 * @param	idx		Index of the mouse axis to smooth, 0 - horizontal, 1 - vertical.
		 *
		 * @returns	Smoothed value.
		 */
		float smoothMouse(float value, UINT32 idx);

		/**
		 * @brief	Default dots per inch reported by the mouse. 
		 *
		 * @note	This should be retrieved from the mouse driver but I am not aware of any decent 
		 *			way of doing it. What this means is that if the user has a mouse with a different 
		 *			DPI then he will need to adjust sensitivity.
		 */
		static const UINT32 MOUSE_DPI;

		/**
		 * @brief	How much does the user need to move the mouse in order to max out the mouse axis
		 *			(either positive or negative), in inches.
		 */
		static const float MOUSE_MAX;

		/**
		 * @brief	Number of seconds the mouse needs to reach the MOUSE_MAX value in, in order to
		 *			max out the axis.
		 */
		static const float MOUSE_MAX_TIME;

		/**
		 * @brief	Minimum number of milliseconds that need to pass before mouse axes are updated again.
		 *			
		 * @note	At extremely high frame rates sampling the mouse too often will introduce jitter. This is because
		 *			mouse will be sampled by the application faster than the hardware reports the samples. This means some
		 *			of the samples will be reported as 0, while in truth mouse could be moving but it just hasn't been sampled.
		 *			So we cannot tell if mouse is actually still, or moving but sample hasn't been updated, and must assume mouse
		 *			is still, which causes jitter as one frame reports mouse as moving and another as still.
		 *
		 *			We could get around this if we knew the exact hardware sampling rate, but we don't.
		 */
		static const float MOUSE_MAX_SAMPLING_RATE;

		OIS::InputManager* mInputManager;
		OIS::Mouse*	mMouse;
		OIS::Keyboard* mKeyboard;
		Vector<GamepadData> mGamepads;

		float mTotalMouseSamplingTime[2];
		UINT32 mTotalMouseNumSamples[2];
		float mMouseZeroTime[2];
		INT32 mMouseSampleAccumulator[2];
		float mMouseSmoothedAxis[2];
		UINT32 mLastMouseUpdateFrame;
		float mMouseSampleCounter;

		UINT64 mTimestampClockOffset;
	};
}
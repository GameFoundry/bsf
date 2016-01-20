//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
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
		virtual bool buttonPressed(const OIS::JoyStickEvent& arg, int button) override;

		/**
		 * @brief	Called by OIS whenever a gamepad/joystick button is released.
		 */
		virtual bool buttonReleased(const OIS::JoyStickEvent& arg, int button) override;

		/**
		 * @brief	Called by OIS whenever a gamepad/joystick axis is moved.
		 */
		virtual bool axisMoved(const OIS::JoyStickEvent& arg, int axis) override;

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
		virtual bool keyPressed(const OIS::KeyEvent& arg) override;

		/**
		 * @brief	Called by OIS whenever a keyboard button is released.
		 */
		virtual bool keyReleased(const OIS::KeyEvent& arg) override;

		/**
		 * @brief	Called by OIS whenever mouse is moved.
		 */
		virtual bool mouseMoved(const OIS::MouseEvent& arg) override;

		/**
		 * @brief	Called by OIS whenever is a mouse button pressed.
		 */
		virtual bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id) override;

		/**
		 * @brief	Called by OIS whenever is a mouse button released
		 */
		virtual bool mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id) override;

		/**
		 * @brief	Called once per frame.
		 *
		 * @note	Internal method.
		 */
		virtual void _update() override;

		/**
		 * @brief	Called whenever the currently focused window changes.
		 *
		 * @note	Internal method.
		 */
		virtual void _inputWindowChanged(const RenderWindow& win) override;

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

		OIS::InputManager* mInputManager;
		OIS::Mouse*	mMouse;
		OIS::Keyboard* mKeyboard;
		Vector<GamepadData> mGamepads;

		float mTotalMouseSamplingTime[2];
		UINT32 mTotalMouseNumSamples[2];
		float mMouseZeroTime[2];
		INT32 mMouseSampleAccumulator[2];
		float mMouseSmoothedAxis[2];
		UINT64 mLastMouseUpdateFrame;

		UINT64 mTimestampClockOffset;
	};
}
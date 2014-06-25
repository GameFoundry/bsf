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

		static const UINT32 MOUSE_SENSITIVITY; /**< Converts arbitrary mouse axis range to [-1, 1] range. */

		OIS::InputManager* mInputManager;
		OIS::Mouse*	mMouse;
		OIS::Keyboard* mKeyboard;
		Vector<GamepadData> mGamepads;

		UINT64 mTimestampClockOffset;
	};
}
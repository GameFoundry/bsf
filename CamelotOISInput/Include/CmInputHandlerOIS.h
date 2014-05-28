#pragma once

#include "CmOISPrerequisites.h"
#include "CmRawInputHandler.h"

#include <OIS/OISEvents.h>
#include <OIS/OISInputManager.h>
#include <OIS/OISKeyboard.h>
#include <OIS/OISMouse.h>

namespace BansheeEngine
{
	/**
	 * @brief	Raw input handler using OIS library for acquiring input.
	 */
	class CM_OIS_EXPORT InputHandlerOIS : public RawInputHandler, public OIS::KeyListener, public OIS::MouseListener
	{
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
		ButtonCode keyCodeToButtonCode(OIS::KeyCode keyCode) const;

		/**
		 * @brief	Converts an OIS mouse button code into engine button code.
		 */
		ButtonCode mouseButtonToButtonCode(OIS::MouseButtonID mouseBtn) const;

	private:
		OIS::InputManager* mInputManager;
		OIS::Mouse*	mMouse;
		OIS::Keyboard* mKeyboard;

		UINT64 mTimestampClockOffset;
	};
}
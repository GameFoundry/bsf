#pragma once

#include "CmPrerequisites.h"
#include "CmPlatform.h"
#include "BsEvent.h"
#include "CmVector2I.h"

namespace BansheeEngine
{
	/**
		* @brief	Represents a specific way of acquiring OS input. InputManager (which provides a higher level input)
		* 			must have at least one OSInputHandler attached. Attach events handler to the provided signals to handle input.
		* 			
		* @note		Unlike RawInputHandler this class receives input from the operating system, and is used for receiving
		* 			text input, cursor position and similar.
		*/
	class CM_EXPORT OSInputHandler
	{
		/**
		 * @brief	Contains information regarding a button state change event.
		 */
		struct ButtonStateChange
		{
			Vector2I cursorPos;
			OSPointerButtonStates btnStates;
			OSMouseButton button;
			bool pressed;
		};

		/**
		 * @brief	Contains information regarding a double click event.
		 */
		struct DoubleClick
		{
			Vector2I cursorPos;
			OSPointerButtonStates btnStates;
		};

	public:
		OSInputHandler();
		virtual ~OSInputHandler();

		/**
		 * @brief	Triggers when user inputs a character. The character might be a result of pressing
		 * 			multiple keys, so character input will not necessarily correspond with button presses.
		 * 			Provide character code of the input character.
		 */
		Event<void(UINT32)> onCharInput;

		/**
		 * @brief	Triggers whenever user scrolls the mouse wheel. Returns the screen
		 * 			position of the mouse cursor and delta amount of mouse scroll (can be negative or positive).
		 */
		Event<void(const Vector2I&, float)> onMouseWheelScrolled;

		/**
		 * @brief	Triggers whenever user moves the mouse cursor.
		 */
		Event<void(const PointerEvent&)> onCursorMoved;

		/**
		 * @brief	Triggers whenever user presses one of the mouse buttons.
		 */
		Event<void(const PointerEvent&)> onCursorPressed;

		/**
		 * @brief	Triggers whenever user releases one of the mouse buttons.
		 */
		Event<void(const PointerEvent&)> onCursorReleased;

		/**
		 * @brief	Triggers when user clicks a mouse button quickly twice in a row.
		 */
		Event<void(const PointerEvent&)> onDoubleClick;

		/**
		 * @brief	Triggers when user inputa a special input command, like commands user
		 * 			for manipulating text input.
		 */
		Event<void(InputCommandType)> onInputCommand;

		/**
		 * @brief	Called once per frame. Capture input here if needed.
		 * 			
		 * @note	Internal method.
		 */
		virtual void _update();

		/**
		 * @brief	Called whenever the active window changes.
		 *
		 * @param	win	Newly active window.
		 * 				
		 * @note	Internal method.
		 */
		virtual void _inputWindowChanged(const RenderWindow& win) {}

	private:
		CM_MUTEX(mOSInputMutex);
		Vector2I mLastCursorPos;
		Vector2I mCursorPosition;
		float mMouseScroll;
		WString mInputString;
		Queue<ButtonStateChange> mButtonStates;
		Queue<DoubleClick> mDoubleClicks;
		Queue<InputCommandType> mInputCommands;
		OSPointerButtonStates mMouseMoveBtnState;

		HEvent mCharInputConn;
		HEvent mCursorMovedConn;
		HEvent mCursorPressedConn;
		HEvent mCursorReleasedConn;
		HEvent mCursorDoubleClickConn;
		HEvent mInputCommandConn;
		HEvent mMouseWheelScrolledConn;

		/**
		 * @brief	Called from the message loop to notify user has entered a character.
		 * 			
		 * @see		onCharInput
		 */
		void charInput(UINT32 character);

		/**
		 * @brief	Called from the message loop to notify user has moved the cursor.
		 * 			
		 * @see		onCursorMoved
		 */
		void cursorMoved(const Vector2I& cursorPos, OSPointerButtonStates& btnStates);

		/**
		 * @brief	Called from the message loop to notify user has pressed a mouse button.
		 * 			
		 * @see		onCursorPressed
		 */
		void cursorPressed(const Vector2I& cursorPos, OSMouseButton button, OSPointerButtonStates& btnStates);

		/**
		 * @brief	Called from the message loop to notify user has released a mouse button.
		 * 			
		 * @see		onCursorReleased
		 */
		void cursorReleased(const Vector2I& cursorPos, OSMouseButton button, OSPointerButtonStates& btnStates);

		/**
		 * @brief	Called from the message loop to notify user has double-clicked a mouse button.
		 * 
		 * @see		onDoubleClick
		 */
		void cursorDoubleClick(const Vector2I& cursorPos, OSPointerButtonStates& btnStates);

		/**
		 * @brief	Called from the message loop to notify user has entered an input command.
		 * 			
		 * @see		onInputCommand
		 */
		void inputCommandEntered(InputCommandType commandType);

		/**
		 * @brief	Called from the message loop to notify user has scrolled the mouse wheel.
		 * 			
		 * @see		onMouseWheelScrolled
		 */
		void mouseWheelScrolled(float scrollPos);
	};
}
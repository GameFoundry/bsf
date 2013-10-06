#pragma once

#include "CmPrerequisites.h"
#include "CmPlatform.h"
#include <boost/signal.hpp>
#include "CmInt2.h"	

namespace CamelotFramework
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
		struct ButtonStateChange
		{
			Int2 cursorPos;
			OSPositionalInputButtonStates btnStates;
			OSMouseButton button;
			bool pressed;
		};

	public:
		OSInputHandler();
		virtual ~OSInputHandler();

		boost::signal<void(UINT32)> onCharInput;
		boost::signal<void(const Int2&, float)> onMouseWheelScrolled;
		boost::signal<void(const PositionalInputEvent&)> onCursorMoved;
		boost::signal<void(const PositionalInputEvent&)> onCursorPressed;
		boost::signal<void(const PositionalInputEvent&)> onCursorReleased;

		/**
			* @brief	Called every frame by InputManager. Capture input here if needed.
			*/
		virtual void update();

		/**
			* @brief	Called by InputManager whenever window in focus changes.
			*/
		virtual void inputWindowChanged(const RenderWindow& win) {}

	private:
		CM_MUTEX(mOSInputMutex);
		Int2 mLastCursorPos;
		Int2 mCursorPosition;
		float mMouseScroll;
		WString mInputString;
		Queue<ButtonStateChange>::type mButtonStates;
		OSPositionalInputButtonStates mMouseMoveBtnState;

		boost::signals::connection mCharInputConn;
		boost::signals::connection mCursorMovedConn;
		boost::signals::connection mCursorPressedConn;
		boost::signals::connection mCursorReleasedConn;
		boost::signals::connection mMouseWheelScrolledConn;

		/**
		 * @brief	Called from the message loop.
		 */
		void charInput(UINT32 character);

		/**
		 * @brief	Called from the message loop.
		 */
		void cursorMoved(const Int2& cursorPos, OSPositionalInputButtonStates& btnStates);

		/**
		 * @brief	Called from the message loop.
		 */
		void cursorPressed(const Int2& cursorPos, OSMouseButton button, OSPositionalInputButtonStates& btnStates);

		/**
		 * @brief	Called from the message loop.
		 */
		void cursorReleased(const Int2& cursorPos, OSMouseButton button, OSPositionalInputButtonStates& btnStates);

		/**
		 * @brief	Called from the message loop.
		 */
		void mouseWheelScrolled(float scrollPos);
	};
}
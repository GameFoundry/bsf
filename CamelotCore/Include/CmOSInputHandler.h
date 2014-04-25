#pragma once

#include "CmPrerequisites.h"
#include "CmPlatform.h"
#include <boost/signal.hpp>
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
		struct ButtonStateChange
		{
			Vector2I cursorPos;
			OSPointerButtonStates btnStates;
			OSMouseButton button;
			bool pressed;
		};

		struct DoubleClick
		{
			Vector2I cursorPos;
			OSPointerButtonStates btnStates;
		};

	public:
		OSInputHandler();
		virtual ~OSInputHandler();

		boost::signal<void(UINT32)> onCharInput;
		boost::signal<void(const Vector2I&, float)> onMouseWheelScrolled;
		boost::signal<void(const PointerEvent&)> onCursorMoved;
		boost::signal<void(const PointerEvent&)> onCursorPressed;
		boost::signal<void(const PointerEvent&)> onCursorReleased;
		boost::signal<void(const PointerEvent&)> onDoubleClick;
		boost::signal<void(InputCommandType)> onInputCommand;

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
		Vector2I mLastCursorPos;
		Vector2I mCursorPosition;
		float mMouseScroll;
		WString mInputString;
		Queue<ButtonStateChange>::type mButtonStates;
		Queue<DoubleClick>::type mDoubleClicks;
		Queue<InputCommandType>::type mInputCommands;
		OSPointerButtonStates mMouseMoveBtnState;

		boost::signals::connection mCharInputConn;
		boost::signals::connection mCursorMovedConn;
		boost::signals::connection mCursorPressedConn;
		boost::signals::connection mCursorReleasedConn;
		boost::signals::connection mCursorDoubleClickConn;
		boost::signals::connection mInputCommandConn;
		boost::signals::connection mMouseWheelScrolledConn;

		/**
		 * @brief	Called from the message loop.
		 */
		void charInput(UINT32 character);

		/**
		 * @brief	Called from the message loop.
		 */
		void cursorMoved(const Vector2I& cursorPos, OSPointerButtonStates& btnStates);

		/**
		 * @brief	Called from the message loop.
		 */
		void cursorPressed(const Vector2I& cursorPos, OSMouseButton button, OSPointerButtonStates& btnStates);

		/**
		 * @brief	Called from the message loop.
		 */
		void cursorReleased(const Vector2I& cursorPos, OSMouseButton button, OSPointerButtonStates& btnStates);

		/**
		 * @brief	Called from the message loop.
		 */
		void cursorDoubleClick(const Vector2I& cursorPos, OSPointerButtonStates& btnStates);

		/**
		 * @brief	Called from the message loop.
		 */
		void inputCommandEntered(InputCommandType commandType);

		/**
		 * @brief	Called from the message loop.
		 */
		void mouseWheelScrolled(float scrollPos);
	};
}
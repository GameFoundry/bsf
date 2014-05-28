#include "CmOSInputHandler.h"
#include "CmPlatform.h"
#include "CmInput.h"
#include "CmMath.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	OSInputHandler::OSInputHandler()
		:mMouseScroll(0.0f)
	{
		mCharInputConn = Platform::onCharInput.connect(std::bind(&OSInputHandler::charInput, this, _1));
		mCursorMovedConn = Platform::onCursorMoved.connect(std::bind(&OSInputHandler::cursorMoved, this, _1, _2));
		mCursorPressedConn = Platform::onCursorButtonPressed.connect(std::bind(&OSInputHandler::cursorPressed, this, _1, _2, _3));
		mCursorReleasedConn = Platform::onCursorButtonReleased.connect(std::bind(&OSInputHandler::cursorReleased, this, _1, _2, _3));
		mCursorDoubleClickConn = Platform::onCursorDoubleClick.connect(std::bind(&OSInputHandler::cursorDoubleClick, this, _1, _2));
		mInputCommandConn = Platform::onInputCommand.connect(std::bind(&OSInputHandler::inputCommandEntered, this, _1));

		mMouseWheelScrolledConn  = Platform::onMouseWheelScrolled.connect(std::bind(&OSInputHandler::mouseWheelScrolled, this, _1));
	}

	OSInputHandler::~OSInputHandler()
	{
		mCharInputConn.disconnect();
		mCursorMovedConn.disconnect();
		mCursorPressedConn.disconnect();
		mCursorReleasedConn.disconnect();
		mCursorDoubleClickConn.disconnect();
		mInputCommandConn.disconnect();
		mMouseWheelScrolledConn.disconnect();
	}

	void OSInputHandler::_update()
	{
		WString inputString;
		Vector2I mousePosition;
		float mouseScroll;
		OSPointerButtonStates mouseMoveBtnState;
		Queue<ButtonStateChange> buttonStates;
		Queue<DoubleClick> doubleClicks;
		Queue<InputCommandType> inputCommands;

		{
			CM_LOCK_MUTEX(mOSInputMutex);
			inputString = mInputString;
			mInputString.clear();

			mousePosition = mCursorPosition;
			mouseScroll = mMouseScroll;
			mMouseScroll = 0.0f;

			mouseMoveBtnState = mMouseMoveBtnState;

			buttonStates = mButtonStates;
			mButtonStates = Queue<ButtonStateChange>();

			inputCommands = mInputCommands;
			mInputCommands = Queue<InputCommandType>();

			doubleClicks = mDoubleClicks;
			mDoubleClicks = Queue<DoubleClick>();
		}

		if(mousePosition != mLastCursorPos || (Math::abs(mouseScroll) > 0.00001f))
		{
			if(!onCursorMoved.empty())
			{
				PointerEvent event;
				event.alt = false;
				event.shift = mouseMoveBtnState.shift;
				event.control = mouseMoveBtnState.ctrl;
				event.buttonStates[0] = mouseMoveBtnState.mouseButtons[0];
				event.buttonStates[1] = mouseMoveBtnState.mouseButtons[1];
				event.buttonStates[2] = mouseMoveBtnState.mouseButtons[2];
				event.mouseWheelScrollAmount = mouseScroll;

				event.type = PointerEventType::CursorMoved;
				event.screenPos = mousePosition;

				onCursorMoved(event);
			}

			mLastCursorPos = mousePosition;
		}

		while(!buttonStates.empty())
		{
			ButtonStateChange& btnState = buttonStates.front();

			PointerEvent event;
			event.alt = false;
			event.shift = btnState.btnStates.shift;
			event.control = btnState.btnStates.ctrl;
			event.buttonStates[0] = btnState.btnStates.mouseButtons[0];
			event.buttonStates[1] = btnState.btnStates.mouseButtons[1];
			event.buttonStates[2] = btnState.btnStates.mouseButtons[2];
			
			switch(btnState.button)
			{
			case OSMouseButton::Left:
				event.button = PointerEventButton::Left;
				break;
			case OSMouseButton::Middle:
				event.button = PointerEventButton::Middle;
				break;
			case OSMouseButton::Right:
				event.button = PointerEventButton::Right;
				break;
			}
			
			event.screenPos = btnState.cursorPos;

			if(btnState.pressed)
			{
				event.type = PointerEventType::ButtonPressed;

				if(!onCursorPressed.empty())
					onCursorPressed(event);
			}
			else
			{
				event.type = PointerEventType::ButtonReleased;

				if(!onCursorReleased.empty())
					onCursorReleased(event);
			}

			buttonStates.pop();
		}

		while(!doubleClicks.empty())
		{
			if(!onDoubleClick.empty())
			{
				DoubleClick& btnState = doubleClicks.front();

				PointerEvent event;
				event.alt = false;
				event.shift = btnState.btnStates.shift;
				event.control = btnState.btnStates.ctrl;
				event.buttonStates[0] = btnState.btnStates.mouseButtons[0];
				event.buttonStates[1] = btnState.btnStates.mouseButtons[1];
				event.buttonStates[2] = btnState.btnStates.mouseButtons[2];
				event.button = PointerEventButton::Left;
				event.screenPos = btnState.cursorPos;
				event.type = PointerEventType::DoubleClick;

				onDoubleClick(event);
			}

			doubleClicks.pop();
		}

		while(!inputCommands.empty())
		{
			if(!onInputCommand.empty())
				onInputCommand(inputCommands.front());

			inputCommands.pop();
		}

		if(!onCharInput.empty())
		{
			for(auto& curChar : inputString)
			{
				onCharInput((UINT32)curChar);
			}
		}
	}

	void OSInputHandler::charInput(UINT32 character)
	{
		CM_LOCK_MUTEX(mOSInputMutex);

		mInputString += character;
	}

	void OSInputHandler::cursorMoved(const Vector2I& cursorPos, OSPointerButtonStates& btnStates)
	{
		CM_LOCK_MUTEX(mOSInputMutex);

		mCursorPosition = cursorPos;
		mMouseMoveBtnState = btnStates;
	}

	void OSInputHandler::cursorPressed(const Vector2I& cursorPos, 
		OSMouseButton button, OSPointerButtonStates& btnStates)
	{
		CM_LOCK_MUTEX(mOSInputMutex);

		mButtonStates.push(ButtonStateChange());
		ButtonStateChange& btnState = mButtonStates.back();

		btnState.cursorPos = cursorPos;
		btnState.button = button;
		btnState.pressed = true;
		btnState.btnStates = btnStates;
	}

	void OSInputHandler::cursorReleased(const Vector2I& cursorPos, 
		OSMouseButton button, OSPointerButtonStates& btnStates)
	{
		CM_LOCK_MUTEX(mOSInputMutex);

		mButtonStates.push(ButtonStateChange());
		ButtonStateChange& btnState = mButtonStates.back();

		btnState.cursorPos = cursorPos;
		btnState.button = button;
		btnState.pressed = false;
		btnState.btnStates = btnStates;
	}

	void OSInputHandler::cursorDoubleClick(const Vector2I& cursorPos, OSPointerButtonStates& btnStates)
	{
		CM_LOCK_MUTEX(mOSInputMutex);

		mDoubleClicks.push(DoubleClick());
		DoubleClick& btnState = mDoubleClicks.back();

		btnState.cursorPos = cursorPos;
		btnState.btnStates = btnStates;
	}

	void OSInputHandler::inputCommandEntered(InputCommandType commandType)
	{
		CM_LOCK_MUTEX(mOSInputMutex);

		mInputCommands.push(commandType);
	}

	void OSInputHandler::mouseWheelScrolled(float scrollPos)
	{
		CM_LOCK_MUTEX(mOSInputMutex);

		mMouseScroll = scrollPos;
	}
}
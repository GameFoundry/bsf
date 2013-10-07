#include "CmOSInputHandler.h"
#include "CmPlatform.h"
#include "CmInput.h"
#include "CmMath.h"

namespace CamelotFramework
{
	OSInputHandler::OSInputHandler()
		:mMouseScroll(0.0f)
	{
		mCharInputConn = Platform::onCharInput.connect(boost::bind(&OSInputHandler::charInput, this, _1));
		mCursorMovedConn = Platform::onCursorMoved.connect(boost::bind(&OSInputHandler::cursorMoved, this, _1, _2));
		mCursorPressedConn = Platform::onCursorButtonPressed.connect(boost::bind(&OSInputHandler::cursorPressed, this, _1, _2, _3));
		mCursorReleasedConn = Platform::onCursorButtonReleased.connect(boost::bind(&OSInputHandler::cursorReleased, this, _1, _2, _3));
		mCursorDoubleClickConn = Platform::onCursorDoubleClick.connect(boost::bind(&OSInputHandler::cursorDoubleClick, this, _1, _2));
		mInputCommandConn = Platform::onInputCommand.connect(boost::bind(&OSInputHandler::inputCommandEntered, this, _1));

		mMouseWheelScrolledConn  = Platform::onMouseWheelScrolled.connect(boost::bind(&OSInputHandler::mouseWheelScrolled, this, _1));
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

	void OSInputHandler::update()
	{
		WString inputString;
		Int2 mousePosition;
		float mouseScroll;
		OSPositionalInputButtonStates mouseMoveBtnState;
		Queue<ButtonStateChange>::type buttonStates;
		Queue<DoubleClick>::type doubleClicks;
		Queue<InputCommandType>::type inputCommands;

		{
			CM_LOCK_MUTEX(mOSInputMutex);
			inputString = mInputString;
			mInputString.clear();

			mousePosition = mCursorPosition;
			mouseScroll = mMouseScroll;
			mMouseScroll = 0.0f;

			mouseMoveBtnState = mMouseMoveBtnState;

			buttonStates = mButtonStates;
			mButtonStates = Queue<ButtonStateChange>::type();

			inputCommands = mInputCommands;
			mInputCommands = Queue<InputCommandType>::type();

			doubleClicks = mDoubleClicks;
			mDoubleClicks = Queue<DoubleClick>::type();
		}

		if(mousePosition != mLastCursorPos || (Math::Abs(mouseScroll) > 0.00001f))
		{
			if(!onCursorMoved.empty())
			{
				PositionalInputEvent event;
				event.alt = false;
				event.shift = mouseMoveBtnState.shift;
				event.control = mouseMoveBtnState.ctrl;
				event.buttonStates[0] = mouseMoveBtnState.mouseButtons[0];
				event.buttonStates[1] = mouseMoveBtnState.mouseButtons[1];
				event.buttonStates[2] = mouseMoveBtnState.mouseButtons[2];
				event.mouseWheelScrollAmount = mouseScroll;

				event.type = PositionalInputEventType::CursorMoved;
				event.screenPos = mousePosition;

				onCursorMoved(event);
			}

			mLastCursorPos = mousePosition;
		}

		while(!buttonStates.empty())
		{
			ButtonStateChange& btnState = buttonStates.front();

			PositionalInputEvent event;
			event.alt = false;
			event.shift = btnState.btnStates.shift;
			event.control = btnState.btnStates.ctrl;
			event.buttonStates[0] = btnState.btnStates.mouseButtons[0];
			event.buttonStates[1] = btnState.btnStates.mouseButtons[1];
			event.buttonStates[2] = btnState.btnStates.mouseButtons[2];
			
			switch(btnState.button)
			{
			case OSMouseButton::Left:
				event.button = PositionalInputEventButton::Left;
				break;
			case OSMouseButton::Middle:
				event.button = PositionalInputEventButton::Middle;
				break;
			case OSMouseButton::Right:
				event.button = PositionalInputEventButton::Right;
				break;
			}
			
			event.screenPos = btnState.cursorPos;

			if(btnState.pressed)
			{
				event.type = PositionalInputEventType::ButtonPressed;

				if(!onCursorPressed.empty())
					onCursorPressed(event);
			}
			else
			{
				event.type = PositionalInputEventType::ButtonReleased;

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

				PositionalInputEvent event;
				event.alt = false;
				event.shift = btnState.btnStates.shift;
				event.control = btnState.btnStates.ctrl;
				event.buttonStates[0] = btnState.btnStates.mouseButtons[0];
				event.buttonStates[1] = btnState.btnStates.mouseButtons[1];
				event.buttonStates[2] = btnState.btnStates.mouseButtons[2];
				event.button = PositionalInputEventButton::Left;
				event.screenPos = btnState.cursorPos;
				event.type = PositionalInputEventType::DoubleClick;

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

	void OSInputHandler::cursorMoved(const Int2& cursorPos, OSPositionalInputButtonStates& btnStates)
	{
		CM_LOCK_MUTEX(mOSInputMutex);

		mCursorPosition = cursorPos;
		mMouseMoveBtnState = btnStates;
	}

	void OSInputHandler::cursorPressed(const Int2& cursorPos, 
		OSMouseButton button, OSPositionalInputButtonStates& btnStates)
	{
		CM_LOCK_MUTEX(mOSInputMutex);

		mButtonStates.push(ButtonStateChange());
		ButtonStateChange& btnState = mButtonStates.back();

		btnState.cursorPos = cursorPos;
		btnState.button = button;
		btnState.pressed = true;
		btnState.btnStates = btnStates;
	}

	void OSInputHandler::cursorReleased(const Int2& cursorPos, 
		OSMouseButton button, OSPositionalInputButtonStates& btnStates)
	{
		CM_LOCK_MUTEX(mOSInputMutex);

		mButtonStates.push(ButtonStateChange());
		ButtonStateChange& btnState = mButtonStates.back();

		btnState.cursorPos = cursorPos;
		btnState.button = button;
		btnState.pressed = false;
		btnState.btnStates = btnStates;
	}

	void OSInputHandler::cursorDoubleClick(const Int2& cursorPos, OSPositionalInputButtonStates& btnStates)
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
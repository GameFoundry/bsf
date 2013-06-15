#include "CmOSInputHandler.h"
#include "CmWindowEventUtilities.h"

namespace CamelotFramework
{
	OSInputHandler::OSInputHandler()
	{
		mCharInputConn = WindowEventUtilities::onCharInput.connect(boost::bind(&OSInputHandler::charInput, this, _1));
		mMouseMovedConn = WindowEventUtilities::onMouseMoved.connect(boost::bind(&OSInputHandler::mouseMoved, this, _1));
	}

	OSInputHandler::~OSInputHandler()
	{
		mCharInputConn.disconnect();
		mMouseMovedConn.disconnect();
	}

	void OSInputHandler::update()
	{
		WString inputString;
		Int2 mousePosition;

		{
			CM_LOCK_MUTEX(mOSInputMutex);
			inputString = mInputString;
			mInputString.clear();

			mousePosition = mMousePosition;
		}

		if(mousePosition != mLastMousePos)
		{
			if(!onMouseMoved.empty())
				onMouseMoved(mousePosition);

			mLastMousePos = mousePosition;
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

	void OSInputHandler::mouseMoved(const Int2& mousePos)
	{
		CM_LOCK_MUTEX(mOSInputMutex);

		mMousePosition = mousePos;
	}
}
#include "CmOSInputHandler.h"
#include "CmWindowEventUtilities.h"
#include "CmMath.h"

namespace CamelotFramework
{
	OSInputHandler::OSInputHandler()
		:mMouseScroll(0.0f)
	{
		mCharInputConn = WindowEventUtilities::onCharInput.connect(boost::bind(&OSInputHandler::charInput, this, _1));
		mMouseMovedConn = WindowEventUtilities::onMouseMoved.connect(boost::bind(&OSInputHandler::mouseMoved, this, _1));
		mMouseWheelScrolledConn  = WindowEventUtilities::onMouseWheelScrolled.connect(boost::bind(&OSInputHandler::mouseWheelScrolled, this, _1));
	}

	OSInputHandler::~OSInputHandler()
	{
		mCharInputConn.disconnect();
		mMouseMovedConn.disconnect();
		mMouseWheelScrolledConn.disconnect();
	}

	void OSInputHandler::update()
	{
		WString inputString;
		Int2 mousePosition;
		float mouseScroll;

		{
			CM_LOCK_MUTEX(mOSInputMutex);
			inputString = mInputString;
			mInputString.clear();

			mousePosition = mMousePosition;
			mouseScroll = mMouseScroll;
			mMouseScroll = 0.0f;
		}

		if(mousePosition != mLastMousePos || (Math::Abs(mouseScroll) > 0.00001f))
		{
			if(!onMouseMoved.empty())
				onMouseMoved(mousePosition, mouseScroll);

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

	void OSInputHandler::mouseWheelScrolled(float scrollPos)
	{
		CM_LOCK_MUTEX(mOSInputMutex);

		mMouseScroll = scrollPos;
	}
}
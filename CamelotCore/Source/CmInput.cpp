#include "CmInput.h"
#include "CmTime.h"
#include "CmMath.h"
#include "CmRect.h"
#include "CmDebug.h"

#include <boost/bind.hpp>

namespace CamelotFramework
{
	const int Input::HISTORY_BUFFER_SIZE = 10; // Size of buffer used for input smoothing
	const float Input::WEIGHT_MODIFIER = 0.5f;

	Input::Input()
		:mSmoothHorizontalAxis(0.0f), mSmoothVerticalAxis(0.0f), mCurrentBufferIdx(0), mMouseLastRel(0, 0),
		mUsingClipRect(false), mClipRect(0, 0, 0, 0), mInputHandler(nullptr)
	{ 
		mHorizontalHistoryBuffer = cm_newN<float>(HISTORY_BUFFER_SIZE);
		mVerticalHistoryBuffer = cm_newN<float>(HISTORY_BUFFER_SIZE);
		mTimesHistoryBuffer = cm_newN<float>(HISTORY_BUFFER_SIZE);

		for(int i = 0; i < HISTORY_BUFFER_SIZE; i++)
		{
			mHorizontalHistoryBuffer[i] = 0.0f;
			mVerticalHistoryBuffer[i] = 0.0f;
			mTimesHistoryBuffer[i] = 0.0f;
		}

		for(int i = 0; i < MB_Count; i++)
			mMouseButtonState[i] = false;

		for(int i = 0; i < KC_Count; i++)
			mKeyState[i] = false;
	}

	Input::~Input()
	{
		cm_deleteN(mHorizontalHistoryBuffer, HISTORY_BUFFER_SIZE);
		cm_deleteN(mVerticalHistoryBuffer, HISTORY_BUFFER_SIZE);
		cm_deleteN(mTimesHistoryBuffer, HISTORY_BUFFER_SIZE);
	}

	void Input::initClipRect(Rect& clipRect)
	{
		mClipRect = clipRect;

		mUsingClipRect = (clipRect.width > 0 && clipRect.height > 0);
	}

	void Input::registerInputHandler(InputHandlerPtr inputHandler)
	{
		if(mInputHandler != inputHandler)
		{
			mInputHandler = inputHandler;

			if(mInputHandler != nullptr)
			{
				mInputHandler->onKeyDown.connect(boost::bind(&Input::keyDown, this, _1));
				mInputHandler->onKeyUp.connect(boost::bind(&Input::keyUp, this, _1));

				mInputHandler->onMouseMoved.connect(boost::bind(&Input::mouseMoved, this, _1));
				mInputHandler->onMouseDown.connect(boost::bind(&Input::mouseDown, this, _1, _2));
				mInputHandler->onMouseUp.connect(boost::bind(&Input::mouseUp, this, _1, _2));
			}
		}
	}

	void Input::update()
	{
		if(mInputHandler == nullptr)
		{
			LOGERR("Input handler not initialized!");
			return;
		}

		mInputHandler->update();

		updateSmoothInput();
	}

	void Input::keyDown(KeyCode keyCode)
	{
		mKeyState[keyCode] = true;
	}

	void Input::keyUp(KeyCode keyCode)
	{
		mKeyState[keyCode] = false;
	}

	void Input::mouseMoved(const MouseEvent& event)
	{
		if(mUsingClipRect)
		{
			if(!mClipRect.contains(event.coords))
				return;
		}

		mMouseLastRel = Int2(-event.relCoords.x, -event.relCoords.y);
	}

	void Input::mouseDown(const MouseEvent& event, MouseButton buttonID)
	{
		if(mUsingClipRect)
		{
			if(!mClipRect.contains(event.coords))
				return;
		}

		mMouseButtonState[buttonID] = true;
	}

	void Input::mouseUp(const MouseEvent& event, MouseButton buttonID)
	{
		if(mUsingClipRect)
		{
			if(!mClipRect.contains(event.coords))
				return;
		}

		mMouseButtonState[buttonID] = false;
	}

	float Input::getHorizontalAxis() const
	{
		return mSmoothHorizontalAxis;
	}

	float Input::getVerticalAxis() const
	{
		return mSmoothVerticalAxis;
	}

	bool Input::isButtonDown(MouseButton button) const
	{
		return mMouseButtonState[button];
	}

	bool Input::isKeyDown(KeyCode keyCode) const
	{
		return mKeyState[keyCode];
	}

	void Input::updateSmoothInput()
	{
		float currentTime = gTime().getTime();

		mHorizontalHistoryBuffer[mCurrentBufferIdx] = (float)mMouseLastRel.x;
		mVerticalHistoryBuffer[mCurrentBufferIdx] = (float)mMouseLastRel.y;
		mTimesHistoryBuffer[mCurrentBufferIdx] = currentTime;

		int i = 0;
		int idx = mCurrentBufferIdx;
		float currentWeight = 1.0f;
		float horizontalTotal = 0.0f;
		float verticalTotal = 0.0f;
		while(i < HISTORY_BUFFER_SIZE)
		{
			float timeWeight = 1.0f - (currentTime - mTimesHistoryBuffer[idx]) * 10.0f;
			if(timeWeight < 0.0f)
				timeWeight = 0.0f;

			horizontalTotal += mHorizontalHistoryBuffer[idx] * currentWeight * timeWeight;
			verticalTotal += mVerticalHistoryBuffer[idx] * currentWeight * timeWeight;

			currentWeight *= WEIGHT_MODIFIER;
			idx = (idx + 1) % HISTORY_BUFFER_SIZE;
			i++;
		}

		mCurrentBufferIdx = (mCurrentBufferIdx + 1) % HISTORY_BUFFER_SIZE;

		mSmoothHorizontalAxis = Math::Clamp(horizontalTotal / HISTORY_BUFFER_SIZE, -1.0f, 1.0f);
		mSmoothVerticalAxis = Math::Clamp(verticalTotal / HISTORY_BUFFER_SIZE, -1.0f, 1.0f);

		mMouseLastRel = Int2(0, 0);
	}

	Input& gInput()
	{
		return Input::instance();
	}
}
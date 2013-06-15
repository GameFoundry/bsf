#include "CmInput.h"
#include "CmTime.h"
#include "CmMath.h"
#include "CmRect.h"
#include "CmDebug.h"
#include "CmRenderWindowManager.h"

#include <boost/bind.hpp>

namespace CamelotFramework
{
	const int Input::HISTORY_BUFFER_SIZE = 10; // Size of buffer used for input smoothing
	const float Input::WEIGHT_MODIFIER = 0.5f;

	Input::Input()
		:mSmoothHorizontalAxis(0.0f), mSmoothVerticalAxis(0.0f), mCurrentBufferIdx(0), mMouseLastRel(0, 0), mRawInputHandler(nullptr)
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

		for(int i = 0; i < BC_Count; i++)
			mKeyState[i] = false;

		mOSInputHandler = cm_shared_ptr<OSInputHandler>();

		mOSInputHandler->onCharInput.connect(boost::bind(&Input::charInput, this, _1));
		mOSInputHandler->onMouseMoved.connect(boost::bind(&Input::mouseMoved, this, _1));

		RenderWindowManager::instance().onFocusGained.connect(boost::bind(&Input::inputWindowChanged, this, _1));
	}

	Input::~Input()
	{
		cm_deleteN(mHorizontalHistoryBuffer, HISTORY_BUFFER_SIZE);
		cm_deleteN(mVerticalHistoryBuffer, HISTORY_BUFFER_SIZE);
		cm_deleteN(mTimesHistoryBuffer, HISTORY_BUFFER_SIZE);
	}

	void Input::registerRawInputHandler(std::shared_ptr<RawInputHandler> inputHandler)
	{
		if(mRawInputHandler != inputHandler)
		{
			mRawInputHandler = inputHandler;

			if(mRawInputHandler != nullptr)
			{
				mRawInputHandler->onButtonDown.connect(boost::bind(&Input::buttonDown, this, _1));
				mRawInputHandler->onButtonUp.connect(boost::bind(&Input::buttonUp, this, _1));

				mRawInputHandler->onAxisMoved.connect(boost::bind(&Input::axisMoved, this, _1, _2));
			}
		}
	}

	void Input::update()
	{
		if(mOSInputHandler == nullptr)
		{
			LOGERR("OS input handler not initialized!");
			return;
		}
		else
			mOSInputHandler->update();

		if(mRawInputHandler == nullptr)
		{
			LOGERR("Raw input handler not initialized!");
			return;
		}
		else
			mRawInputHandler->update();

		updateSmoothInput();
	}

	void Input::inputWindowChanged(RenderWindow& win)
	{
		if(mRawInputHandler != nullptr)
			mRawInputHandler->inputWindowChanged(win);

		if(mOSInputHandler != nullptr)
			mOSInputHandler->inputWindowChanged(win);
	}

	void Input::buttonDown(ButtonCode code)
	{
		mKeyState[code & 0x0000FFFF] = true;

		if(!onKeyDown.empty())
		{
			ButtonEvent btnEvent;
			btnEvent.buttonCode = code;

			onKeyDown(btnEvent);
		}
	}

	void Input::buttonUp(ButtonCode code)
	{
		mKeyState[code & 0x0000FFFF] = false;

		if(!onKeyUp.empty())
		{
			ButtonEvent btnEvent;
			btnEvent.buttonCode = code;

			onKeyUp(btnEvent);
		}
	}

	void Input::axisMoved(const RawAxisState& state, RawInputAxis axis)
	{
		if(axis == RawInputAxis::Mouse_XY)
			mMouseLastRel = Int2(-state.rel.x, -state.rel.y);

		mAxes[(int)axis] = state;
	}

	void Input::mouseMoved(const Int2& screenPos)
	{
		mMouseAbsPos = screenPos;

		if(!onMouseMoved.empty())
		{
			MouseEvent mouseEvent;
			mouseEvent.screenPos = screenPos;

			onMouseMoved(mouseEvent);
		}
	}

	void Input::charInput(UINT32 chr)
	{
		if(!onCharInput.empty())
		{
			TextInputEvent textInputEvent;
			textInputEvent.textChar = chr;

			onCharInput(textInputEvent);
		}
	}

	float Input::getHorizontalAxis() const
	{
		return mSmoothHorizontalAxis;
	}

	float Input::getVerticalAxis() const
	{
		return mSmoothVerticalAxis;
	}

	bool Input::isButtonDown(ButtonCode button) const
	{
		return mKeyState[button & 0x0000FFFF];
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
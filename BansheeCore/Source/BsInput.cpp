#include "BsInput.h"
#include "BsTime.h"
#include "BsMath.h"
#include "BsRect2I.h"
#include "BsDebug.h"
#include "BsRenderWindowManager.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	const int Input::HISTORY_BUFFER_SIZE = 10; // Size of buffer used for input smoothing
	const float Input::WEIGHT_MODIFIER = 0.5f;

	Input::DeviceData::DeviceData()
	{
		for (int i = 0; i < BC_Count; i++)
			keyStates[i] = ButtonState::Off;
	}

	Input::Input()
		:mLastPositionSet(false)
	{ 
		mOSInputHandler = bs_shared_ptr<OSInputHandler>();

		mOSInputHandler->onCharInput.connect(std::bind(&Input::charInput, this, _1));
		mOSInputHandler->onCursorMoved.connect(std::bind(&Input::cursorMoved, this, _1));
		mOSInputHandler->onCursorPressed.connect(std::bind(&Input::cursorPressed, this, _1));
		mOSInputHandler->onCursorReleased.connect(std::bind(&Input::cursorReleased, this, _1));
		mOSInputHandler->onDoubleClick.connect(std::bind(&Input::cursorDoubleClick, this, _1));
		mOSInputHandler->onInputCommand.connect(std::bind(&Input::inputCommandEntered, this, _1));

		RenderWindowManager::instance().onFocusGained.connect(std::bind(&Input::inputWindowChanged, this, _1));
	}

	Input::~Input()
	{ }

	void Input::_registerRawInputHandler(std::shared_ptr<RawInputHandler> inputHandler)
	{
		if(mRawInputHandler != inputHandler)
		{
			mRawInputHandler = inputHandler;

			if(mRawInputHandler != nullptr)
			{
				mRawInputHandler->onButtonDown.connect(std::bind(&Input::buttonDown, this, _1, _2, _3));
				mRawInputHandler->onButtonUp.connect(std::bind(&Input::buttonUp, this, _1, _2, _3));

				mRawInputHandler->onAxisMoved.connect(std::bind(&Input::axisMoved, this, _1, _2, _3));
			}
		}
	}

	void Input::_update()
	{
		// Toggle states only remain active for a single frame before they are transitioned
		// into permanent state

		for (auto& deviceData : mDevices)
		{
			for (UINT32 i = 0; i < BC_Count; i++)
			{
				if (deviceData.keyStates[i] == ButtonState::ToggledOff)
					deviceData.keyStates[i] = ButtonState::Off;
				else if (deviceData.keyStates[i] == ButtonState::ToggledOn)
					deviceData.keyStates[i] = ButtonState::On;
			}
		}

		mPointerDelta = Vector2I::ZERO; // Reset delta in case we don't receive any mouse input this frame

		if(mRawInputHandler == nullptr)
		{
			LOGERR("Raw input handler not initialized!");
			return;
		}
		else
			mRawInputHandler->_update();

		if(mOSInputHandler == nullptr)
		{
			LOGERR("OS input handler not initialized!");
			return;
		}
		else
			mOSInputHandler->_update();
	}

	void Input::inputWindowChanged(RenderWindow& win)
	{
		if(mRawInputHandler != nullptr)
			mRawInputHandler->_inputWindowChanged(win);

		if(mOSInputHandler != nullptr)
			mOSInputHandler->_inputWindowChanged(win);
	}

	void Input::buttonDown(UINT32 deviceIdx, ButtonCode code, UINT64 timestamp)
	{
		while (deviceIdx >= (UINT32)mDevices.size())
			mDevices.push_back(DeviceData());

		mDevices[deviceIdx].keyStates[code & 0x0000FFFF] = ButtonState::ToggledOn;

		if(!onButtonDown.empty())
		{
			ButtonEvent btnEvent;
			btnEvent.buttonCode = code;
			btnEvent.timestamp = timestamp;
			btnEvent.deviceIdx = deviceIdx;

			onButtonDown(btnEvent);
		}
	}

	void Input::buttonUp(UINT32 deviceIdx, ButtonCode code, UINT64 timestamp)
	{
		while (deviceIdx >= (UINT32)mDevices.size())
			mDevices.push_back(DeviceData());

		mDevices[deviceIdx].keyStates[code & 0x0000FFFF] = ButtonState::ToggledOff;

		if(!onButtonUp.empty())
		{
			ButtonEvent btnEvent;
			btnEvent.buttonCode = code;
			btnEvent.timestamp = timestamp;
			btnEvent.deviceIdx = deviceIdx;

			onButtonUp(btnEvent);
		}
	}

	void Input::axisMoved(UINT32 deviceIdx, const RawAxisState& state, UINT32 axis)
	{
		while (deviceIdx >= (UINT32)mDevices.size())
			mDevices.push_back(DeviceData());

		Vector<RawAxisState>& axes = mDevices[deviceIdx].axes;
		while (axis >= (UINT32)axes.size())
			axes.push_back(RawAxisState());

		mDevices[deviceIdx].axes[axis] = state;
	}

	void Input::cursorMoved(const PointerEvent& event)
	{
		if(!onPointerMoved.empty())
			onPointerMoved(event);

		if (mLastPositionSet)
			mPointerDelta = event.screenPos - mPointerPosition;

		mPointerPosition = event.screenPos;
		mLastPositionSet = true;
	}

	void Input::cursorPressed(const PointerEvent& event)
	{
		if(!onPointerPressed.empty())
			onPointerPressed(event);
	}

	void Input::cursorReleased(const PointerEvent& event)
	{
		if(!onPointerReleased.empty())
			onPointerReleased(event);
	}

	void Input::cursorDoubleClick(const PointerEvent& event)
	{
		if(!onPointerDoubleClick.empty())
			onPointerDoubleClick(event);
	}

	void Input::inputCommandEntered(InputCommandType commandType)
	{
		if(!onInputCommand.empty())
			onInputCommand(commandType);
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

	float Input::getAxisValue(UINT32 type, UINT32 deviceIdx) const
	{
		if (deviceIdx >= (UINT32)mDevices.size())
			return 0.0f;

		const Vector<RawAxisState>& axes = mDevices[deviceIdx].axes;
		if (type >= (UINT32)axes.size())
			return 0.0f;

		return axes[type].abs;
	}

	bool Input::isButtonHeld(ButtonCode button, UINT32 deviceIdx) const
	{
		if (deviceIdx >= (UINT32)mDevices.size())
			return false;

		return mDevices[deviceIdx].keyStates[button & 0x0000FFFF] == ButtonState::On || 
			mDevices[deviceIdx].keyStates[button & 0x0000FFFF] == ButtonState::ToggledOn;
	}

	bool Input::isButtonUp(ButtonCode button, UINT32 deviceIdx) const
	{
		if (deviceIdx >= (UINT32)mDevices.size())
			return false;

		return mDevices[deviceIdx].keyStates[button & 0x0000FFFF] == ButtonState::ToggledOff;
	}

	bool Input::isButtonDown(ButtonCode button, UINT32 deviceIdx) const
	{
		if (deviceIdx >= (UINT32)mDevices.size())
			return false;

		return mDevices[deviceIdx].keyStates[button & 0x0000FFFF] == ButtonState::ToggledOn;
	}

	Vector2I Input::getPointerPosition() const
	{
		return mPointerPosition;
	}

	void Input::setMouseSmoothing(bool enable)
	{
		mRawInputHandler->setMouseSmoothing(enable);
	}

	Input& gInput()
	{
		return Input::instance();
	}
}
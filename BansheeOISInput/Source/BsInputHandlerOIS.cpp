#include "BsInputHandlerOIS.h"
#include "BsVector2I.h"
#include "OIS/OISException.h"
#include "BsRenderWindow.h"
#include "BsTime.h"
#include "BsMath.h"

namespace BansheeEngine
{
	const UINT32 InputHandlerOIS::MOUSE_SENSITIVITY = 1;

	GamepadEventListener::GamepadEventListener(InputHandlerOIS* parentHandler, UINT32 joystickIdx)
		:mParentHandler(parentHandler), mGamepadIdx(joystickIdx)
	{ }

	bool GamepadEventListener::buttonPressed(const OIS::JoyStickEvent& arg, int button)
	{
		ButtonCode bc = InputHandlerOIS::gamepadButtonToButtonCode(button);

		mParentHandler->onButtonDown(mGamepadIdx, bc, 0); // TODO - No timestamps
		return true;
	}

	bool GamepadEventListener::buttonReleased(const OIS::JoyStickEvent& arg, int button)
	{
		ButtonCode bc = InputHandlerOIS::gamepadButtonToButtonCode(button);

		mParentHandler->onButtonUp(mGamepadIdx, bc, 0); // TODO - No timestamps
		return true;
	}

	bool GamepadEventListener::axisMoved(const OIS::JoyStickEvent& arg, int axis)
	{
		// Move axis values into [-1.0f, 1.0f] range
		float axisRange = Math::abs((float)OIS::JoyStick::MAX_AXIS) + Math::abs((float)OIS::JoyStick::MIN_AXIS);

		INT32 axisRel = arg.state.mAxes[axis].rel;
		INT32 axisAbs = arg.state.mAxes[axis].abs;

		RawAxisState axisState;
		axisState.rel = ((axisRel + OIS::JoyStick::MIN_AXIS) / axisRange) * 2.0f - 1.0f;
		axisState.abs = ((axisAbs + OIS::JoyStick::MIN_AXIS) / axisRange) * 2.0f - 1.0f;

		mParentHandler->onAxisMoved(mGamepadIdx, axisState, (UINT32)axis);

		return true;
	}

	InputHandlerOIS::InputHandlerOIS(unsigned int hWnd)
		:mInputManager(nullptr), mKeyboard(nullptr), mMouse(nullptr), mTimestampClockOffset(0)
	{
		OIS::ParamList pl;
		std::ostringstream windowHndStr;
		windowHndStr << hWnd;
		pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

#if defined BS_PLATFORM == BS_PLATFORM_WIN32
		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
		pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
		pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
#elif defined BS_PLATFORM == BS_PLATFORM_LINUX || BS_PLATFORM == BS_PLATFORM_APPLE
		pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
		pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
		pl.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
		pl.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
#endif
		try
		{
			mInputManager = OIS::InputManager::createInputSystem(pl);
		}
		catch(OIS::Exception &e)
		{
			std::cout << e.eText << std::endl;
		}

		if (mInputManager->getNumberOfDevices(OIS::OISKeyboard) > 0)
		{
			mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
			mKeyboard->setEventCallback(this);
		}

		if (mInputManager->getNumberOfDevices(OIS::OISMouse) > 0)
		{
			mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));
			mMouse->setEventCallback(this);
		}

		UINT32 numGamepads = mInputManager->getNumberOfDevices(OIS::OISJoyStick);
		for (UINT32 i = 0; i < numGamepads; i++)
		{
			mGamepads.push_back(GamepadData());
			GamepadData& gamepadData = mGamepads.back();

			gamepadData.gamepad = static_cast<OIS::JoyStick*>(mInputManager->createInputObject(OIS::OISJoyStick, true));
			gamepadData.listener = bs_new<GamepadEventListener>(this, i);
		}

		// OIS reports times since system start but we use time since program start
		mTimestampClockOffset = gTime().getStartTimeMs();
	}

	InputHandlerOIS::~InputHandlerOIS()
	{
		if(mInputManager)
		{		
			if(mMouse != nullptr)
				mInputManager->destroyInputObject(mMouse);

			if(mKeyboard != nullptr)
				mInputManager->destroyInputObject(mKeyboard);

			for (auto& gamepadData : mGamepads)
			{
				mInputManager->destroyInputObject(gamepadData.gamepad);
				bs_delete(gamepadData.listener);
			}

			OIS::InputManager::destroyInputSystem(mInputManager);
			mInputManager = nullptr;
		}
	}

	void InputHandlerOIS::_update()
	{
		if (mMouse != nullptr)
			mMouse->capture();

		if (mKeyboard != nullptr)
			mKeyboard->capture();

		for (auto& gamepadData : mGamepads)
		{
			gamepadData.gamepad->capture();
		}
	}

	void InputHandlerOIS::_inputWindowChanged(const RenderWindow& win)
	{
		unsigned long long hWnd;
		win.getCustomAttribute("WINDOW", &hWnd);

		std::string normalString = toString((UINT64)hWnd).c_str();
		mKeyboard->setCaptureContext(normalString);
		mMouse->setCaptureContext(normalString);
	}

	bool InputHandlerOIS::keyPressed(const OIS::KeyEvent &arg)
	{
		onButtonDown(0, keyCodeToButtonCode(arg.key), arg.timestamp - mTimestampClockOffset);
		return true;
	}

	bool InputHandlerOIS::keyReleased(const OIS::KeyEvent& arg)
	{
		onButtonUp(0, keyCodeToButtonCode(arg.key), arg.timestamp - mTimestampClockOffset);
		return true;
	}

	bool InputHandlerOIS::mouseMoved(const OIS::MouseEvent& arg)
	{
		RawAxisState xState;
		xState.rel = Math::clamp(arg.state.X.rel / (float)MOUSE_SENSITIVITY, -1.0f, 1.0f);
		xState.abs = xState.rel; // Abs value irrelevant for mouse
		
		onAxisMoved(0, xState, (UINT32)InputAxis::MouseX);

		RawAxisState yState;
		yState.rel = Math::clamp(arg.state.Y.rel / (float)MOUSE_SENSITIVITY, -1.0f, 1.0f);
		yState.abs = yState.rel; // Abs value irrelevant for mouse

		onAxisMoved(0, yState, (UINT32)InputAxis::MouseY);

		RawAxisState zState;
		zState.abs = (float)arg.state.Z.abs;
		zState.rel = (float)arg.state.Z.rel;

		onAxisMoved(0, zState, (UINT32)InputAxis::MouseZ);

		return true;
	}

	bool InputHandlerOIS::mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
	{
		onButtonDown(0, mouseButtonToButtonCode(id), arg.timestamp - mTimestampClockOffset);

		return true;
	}

	bool InputHandlerOIS::mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
	{
		onButtonUp(0, mouseButtonToButtonCode(id), arg.timestamp - mTimestampClockOffset);

		return true;
	}

	ButtonCode InputHandlerOIS::keyCodeToButtonCode(OIS::KeyCode keyCode)
	{
		return (ButtonCode)keyCode;
	}

	ButtonCode InputHandlerOIS::mouseButtonToButtonCode(OIS::MouseButtonID mouseBtn)
	{
		return (ButtonCode)(((int)mouseBtn + BC_NumKeys) | 0x80000000);
	}

	ButtonCode InputHandlerOIS::gamepadButtonToButtonCode(INT32 joystickCode)
	{
		// TODO
		return BC_0;
	}
}
#include "CmInputHandlerOIS.h"
#include "CmVector2I.h"
#include "OIS/OISException.h"
#include "CmRenderWindow.h"
#include "CmTime.h"

namespace BansheeEngine
{
	InputHandlerOIS::InputHandlerOIS(unsigned int hWnd)
		:mInputManager(nullptr), mKeyboard(nullptr), mMouse(nullptr), mTimestampClockOffset(0)
	{
		OIS::ParamList pl;
		std::ostringstream windowHndStr;
		windowHndStr << hWnd;
		pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

#if defined CM_PLATFORM == CM_PLATFORM_WIN32
		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
		pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
		pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
#elif defined CM_PLATFORM == CM_PLATFORM_LINUX || CM_PLATFORM == CM_PLATFORM_APPLE
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

		mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
		mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));

		// OIS reports times since system start but we use time since program start
		mTimestampClockOffset = gTime().getStartTimeMs();

		mMouse->setEventCallback(this);
		mKeyboard->setEventCallback(this);
	}

	InputHandlerOIS::~InputHandlerOIS()
	{
		if(mInputManager)
		{		
			if(mMouse)
				mInputManager->destroyInputObject(mMouse);

			if(mKeyboard)
				mInputManager->destroyInputObject(mKeyboard);

			OIS::InputManager::destroyInputSystem(mInputManager);
			mInputManager = nullptr;
		}
	}

	void InputHandlerOIS::_update()
	{
		mMouse->capture();
		mKeyboard->capture();
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
		onButtonDown(keyCodeToButtonCode(arg.key), arg.timestamp - mTimestampClockOffset);
		return true;
	}

	bool InputHandlerOIS::keyReleased(const OIS::KeyEvent& arg)
	{
		onButtonUp(keyCodeToButtonCode(arg.key), arg.timestamp - mTimestampClockOffset);
		return true;
	}

	bool InputHandlerOIS::mouseMoved(const OIS::MouseEvent& arg)
	{
		RawAxisState xyState;
		xyState.abs = Vector2I(arg.state.X.abs, arg.state.Y.abs);
		xyState.rel = Vector2I(arg.state.X.rel, arg.state.Y.rel);

		onAxisMoved(xyState, RawInputAxis::Mouse_XY);

		RawAxisState zState;
		zState.abs = Vector2I(arg.state.Z.abs, 0);
		zState.rel = Vector2I(arg.state.Z.rel, 0);

		onAxisMoved(zState, RawInputAxis::Mouse_Z);

		return true;
	}

	bool InputHandlerOIS::mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
	{
		onButtonDown(mouseButtonToButtonCode(id), arg.timestamp - mTimestampClockOffset);

		return true;
	}

	bool InputHandlerOIS::mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
	{
		onButtonUp(mouseButtonToButtonCode(id), arg.timestamp - mTimestampClockOffset);

		return true;
	}

	ButtonCode InputHandlerOIS::keyCodeToButtonCode(OIS::KeyCode keyCode) const
	{
		return (ButtonCode)keyCode;
	}

	ButtonCode InputHandlerOIS::mouseButtonToButtonCode(OIS::MouseButtonID mouseBtn) const
	{
		return (ButtonCode)(((int)mouseBtn + BC_NumKeys) | 0x80000000);
	}
}
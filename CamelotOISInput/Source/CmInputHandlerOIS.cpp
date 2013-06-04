#include "CmInputHandlerOIS.h"
#include "CmInt2.h"
#include "OIS/OISException.h"

namespace CamelotFramework
{
	InputHandlerOIS::InputHandlerOIS(unsigned int hWnd)
		:mInputManager(nullptr), mKeyboard(nullptr), mMouse(nullptr)
	{
		OIS::ParamList pl;
		std::ostringstream windowHndStr;
		windowHndStr << hWnd;
		pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

#if defined CM_PLATFORM == CM_PLATFORM_WIN32
		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_BACKGROUND" )));
		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
		pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_BACKGROUND")));
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

	void InputHandlerOIS::update()
	{
		mMouse->capture();
		mKeyboard->capture();
	}

	bool InputHandlerOIS::keyPressed(const OIS::KeyEvent &arg)
	{
		onKeyDown((KeyCode)(int)arg.key);

		return true;
	}

	bool InputHandlerOIS::keyReleased(const OIS::KeyEvent& arg)
	{
		onKeyUp((KeyCode)(int)arg.key);

		return true;
	}

	bool InputHandlerOIS::mouseMoved(const OIS::MouseEvent& arg)
	{
		MouseEvent event;
		event.coords = Int2(arg.state.X.abs, arg.state.Y.abs);
		event.relCoords = Int2(arg.state.X.rel, arg.state.Y.rel);
		event.z = arg.state.Z.abs;
		event.relZ = arg.state.Z.rel;
		onMouseMoved(event);

		return true;
	}

	bool InputHandlerOIS::mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
	{
		MouseEvent event;
		event.coords = Int2(arg.state.X.abs, arg.state.Y.abs);
		event.relCoords = Int2(arg.state.X.rel, arg.state.Y.rel);
		event.z = arg.state.Z.abs;
		event.relZ = arg.state.Z.rel;
		onMouseDown(event, (MouseButton)(int)id);

		return true;
	}

	bool InputHandlerOIS::mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
	{
		MouseEvent event;
		event.coords = Int2(arg.state.X.abs, arg.state.Y.abs);
		event.relCoords = Int2(0, 0);
		event.z = arg.state.Z.abs;
		onMouseUp(event, (MouseButton)(int)id);

		return true;
	}
}
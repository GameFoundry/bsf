#pragma once

#include "CmOISPrerequisites.h"
#include "CmRawInputHandler.h"

#include <OIS/OISEvents.h>
#include <OIS/OISInputManager.h>
#include <OIS/OISKeyboard.h>
#include <OIS/OISMouse.h>

namespace BansheeEngine
{
	class CM_OIS_EXPORT InputHandlerOIS : public RawInputHandler, public OIS::KeyListener, public OIS::MouseListener
	{
	public:
		InputHandlerOIS(unsigned int hWnd);
		virtual ~InputHandlerOIS();

	private:
		OIS::InputManager*	mInputManager;
		OIS::Mouse*			mMouse;
		OIS::Keyboard*		mKeyboard;

		UINT64				mTimestampClockOffset;

		virtual bool keyPressed(const OIS::KeyEvent& arg);
		virtual bool keyReleased(const OIS::KeyEvent& arg);
		virtual bool mouseMoved(const OIS::MouseEvent& arg);
		virtual bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
		virtual bool mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id);

		virtual void update();
		virtual void inputWindowChanged(const RenderWindow& win);

		ButtonCode keyCodeToButtonCode(OIS::KeyCode keyCode) const;
		ButtonCode mouseButtonToButtonCode(OIS::MouseButtonID mouseBtn) const;
	};
}
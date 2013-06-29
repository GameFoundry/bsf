#include "BsGUIButtonEvent.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIKeyEvent::GUIKeyEvent()
		:mType(GUIKeyEventType::KeyDown), mKey(BC_0), mShift(false), mCtrl(false), mAlt(false)
	{

	}

	GUIKeyEvent::GUIKeyEvent(bool shift, bool ctrl, bool alt)
		:mType(GUIKeyEventType::KeyDown), mKey(BC_0), mShift(shift), mCtrl(ctrl), mAlt(alt)
	{

	}

	void GUIKeyEvent::setKeyDownData(ButtonCode key)
	{
		mType = GUIKeyEventType::KeyDown;
		mKey = key;
		mInputChar = 0;
	}

	void GUIKeyEvent::setKeyUpData(ButtonCode key)
	{
		mType = GUIKeyEventType::KeyUp;
		mKey = key;
		mInputChar = 0;
	}

	void GUIKeyEvent::setTextInputData(UINT32 inputChar)
	{
		mType = GUIKeyEventType::TextInput;
		mKey = BC_0;
		mInputChar = inputChar;
	}
}
#include "BsGUIButtonEvent.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIButtonEvent::GUIButtonEvent()
		:mType(GUIKeyEventType::KeyDown), mKey(BC_0), mShift(false), mCtrl(false), mAlt(false)
	{

	}

	GUIButtonEvent::GUIButtonEvent(bool shift, bool ctrl, bool alt)
		:mType(GUIKeyEventType::KeyDown), mKey(BC_0), mShift(shift), mCtrl(ctrl), mAlt(alt)
	{

	}

	void GUIButtonEvent::setKeyDownData(ButtonCode key)
	{
		mType = GUIKeyEventType::KeyDown;
		mKey = key;
		mInputChar = 0;
	}

	void GUIButtonEvent::setKeyUpData(ButtonCode key)
	{
		mType = GUIKeyEventType::KeyUp;
		mKey = key;
		mInputChar = 0;
	}

	void GUIButtonEvent::setTextInputData(UINT32 inputChar)
	{
		mType = GUIKeyEventType::TextInput;
		mKey = BC_0;
		mInputChar = inputChar;
	}
}
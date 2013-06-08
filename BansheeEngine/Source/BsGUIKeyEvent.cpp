#include "BsGUIKeyEvent.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIKeyEvent::GUIKeyEvent()
		:mType(GUIKeyEventType::KeyDown), mKey(KC_0)
	{

	}

	void GUIKeyEvent::setKeyDownData(KeyCode key)
	{
		mType = GUIKeyEventType::KeyDown;
		mKey = key;
		mInputString = "";
	}

	void GUIKeyEvent::setKeyUpData(KeyCode key)
	{
		mType = GUIKeyEventType::KeyUp;
		mKey = key;
		mInputString = "";
	}

	void GUIKeyEvent::setTextInputData(const WString& string)
	{
		mType = GUIKeyEventType::TextInput;
		mKey = KC_0;
		mInputString = string;
	}
}
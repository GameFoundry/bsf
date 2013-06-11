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
		mInputChar = 0;
	}

	void GUIKeyEvent::setKeyUpData(KeyCode key)
	{
		mType = GUIKeyEventType::KeyUp;
		mKey = key;
		mInputChar = 0;
	}

	void GUIKeyEvent::setTextInputData(UINT32 inputChar)
	{
		mType = GUIKeyEventType::TextInput;
		mKey = KC_0;
		mInputChar = inputChar;
	}
}
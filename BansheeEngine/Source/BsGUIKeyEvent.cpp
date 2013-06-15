#include "BsGUIKeyEvent.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIKeyEvent::GUIKeyEvent()
		:mType(GUIKeyEventType::KeyDown), mKey(BC_0)
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
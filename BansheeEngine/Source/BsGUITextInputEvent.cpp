#include "BsGUITextInputEvent.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUITextInputEvent::GUITextInputEvent()
		:mInputChar(0)
	{

	}

	void GUITextInputEvent::setData(UINT32 inputChar)
	{
		mInputChar = inputChar;
	}
}
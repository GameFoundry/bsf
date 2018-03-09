//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "GUI/BsGUITextInputEvent.h"

namespace bs
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
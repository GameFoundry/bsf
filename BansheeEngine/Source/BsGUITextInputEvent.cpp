//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsGUITextInputEvent.h"

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
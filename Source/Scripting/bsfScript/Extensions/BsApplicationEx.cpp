//********************************* bs::framework - Copyright 2018-2019 Marko Pintera ************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Extensions/BsApplicationEx.h"

using namespace std::placeholders;

namespace bs
{
	void ApplicationEx::startUp(const START_UP_DESC& desc)
	{
		Application::startUp(desc);
	}

	void ApplicationEx::startUp(VideoMode videoMode, const String& title, bool fullscreen)
	{
		Application::startUp(videoMode, title, fullscreen);
	}

	void ApplicationEx::runMainLoop()
	{
		Application::instance().runMainLoop();
	}

	void ApplicationEx::shutDown()
	{
		Application::shutDown();
	}
}

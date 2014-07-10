//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include <windows.h>
#include <BsGLPrerequisites.h>

namespace BansheeEngine
{
    class Win32GLSupport;
    class Win32Window;
    class Win32Context;

	/**
	 * @brief	Retrieves last Windows API error and returns a description of it.
	 */
	String translateWGLError();
}
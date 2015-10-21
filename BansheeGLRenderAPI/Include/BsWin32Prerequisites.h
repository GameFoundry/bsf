#pragma once

#include <windows.h>
#include <BsGLPrerequisites.h>

namespace BansheeEngine
{
    class Win32GLSupport;
    class Win32RenderWindow;
    class Win32Context;
	class Win32RenderWindowCore;

	/**
	 * @brief	Retrieves last Windows API error and returns a description of it.
	 */
	String translateWGLError();
}
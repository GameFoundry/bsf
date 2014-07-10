//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#if BS_PLATFORM == BS_PLATFORM_WIN32

#include "BsWin32GLSupport.h"
#include "BsWin32VideoModeInfo.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Helper method that returns a platform specific GL support object.
	 */
	GLSupport* getGLSupport()
	{
		return bs_new<Win32GLSupport>();
	}
};

#endif
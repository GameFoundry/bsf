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
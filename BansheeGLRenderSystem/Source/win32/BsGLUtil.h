#pragma once

#include "BsWin32GLSupport.h"
#include "BsWin32VideoModeInfo.h"

namespace BansheeEngine 
{
	GLSupport* getGLSupport()
	{
		return bs_new<Win32GLSupport>();
	}
};

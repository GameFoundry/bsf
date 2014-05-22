#pragma once

#include "CmWin32GLSupport.h"
#include "CmWin32VideoModeInfo.h"

namespace BansheeEngine 
{
	GLSupport* getGLSupport()
	{
		return cm_new<Win32GLSupport>();
	}
};

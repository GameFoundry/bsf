#pragma once

#include "CmPrerequisitesUtil.h"

//Bring in the specific platform's header file
#if CM_PLATFORM == CM_PLATFORM_WIN32
# include "Win32/CmTimerImp.h"
#endif

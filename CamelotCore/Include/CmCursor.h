#pragma once

#include "CmPrerequisites.h"

//Bring in the specific platform's header file
#if CM_PLATFORM == CM_PLATFORM_WIN32
# include "Win32/CmCursorImpl.h"
#elif (CM_PLATFORM == CM_PLATFORM_LINUX)
# include "GLX/CmCursorImpl.h"
#elif CM_PLATFORM == CM_PLATFORM_APPLE
# include "OSX/CmCursorImpl.h"
#endif

#define WIN32_LEAN_AND_MEAN
#if !defined(NOMINMAX) && defined(_MSC_VER)
#	define NOMINMAX // Required to stop windows.h messing up std::min
#endif
#include <windows.h>
#include <windowsx.h>

#define WM_CM_SETCAPTURE WM_USER + 101
#define WM_CM_RELEASECAPTURE WM_USER + 102
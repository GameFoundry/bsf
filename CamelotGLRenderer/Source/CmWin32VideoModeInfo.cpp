#include "CmWin32VideoModeInfo.h"
#include "CmMath.h"
#include "CmException.h"

namespace BansheeEngine
{
	BOOL CALLBACK monitorEnumCallback(HMONITOR hMonitor, HDC hdc, LPRECT rect, LPARAM lParam)
	{
		Vector<VideoOutputInfo*>* outputInfos = (Vector<VideoOutputInfo*>*)lParam;
		outputInfos->push_back(cm_new<Win32VideoOutputInfo>(hMonitor));

		return TRUE;
	};

	Win32VideoModeInfo::Win32VideoModeInfo()
	{
		EnumDisplayMonitors(0, nullptr, &monitorEnumCallback, (LPARAM)&mOutputs);
	}

	Win32VideoOutputInfo::Win32VideoOutputInfo(HMONITOR monitorHandle)
		:mMonitorHandle(monitorHandle)
	{
		MONITORINFOEX monitorInfo;

		memset(&monitorInfo, 0, sizeof(MONITORINFOEX));
		monitorInfo.cbSize = sizeof(MONITORINFOEX);
		GetMonitorInfo(mMonitorHandle, &monitorInfo);

		mName = monitorInfo.szDevice;

		DEVMODE devMode;
		devMode.dmSize = sizeof(DEVMODE);
		devMode.dmDriverExtra = 0;

		UINT32 i = 0;
		while (EnumDisplaySettings(monitorInfo.szDevice, i++, &devMode))
		{
			bool foundVideoMode = false;
			for (auto videoMode : mVideoModes)
			{
				Win32VideoMode* win32VideoMode = static_cast<Win32VideoMode*>(videoMode);

				if (win32VideoMode->mWidth == devMode.dmPelsWidth && win32VideoMode->mHeight == devMode.dmPelsHeight)
				{
					bool foundRefreshRate = false;
					for (auto refreshRate : win32VideoMode->mRefreshRates)
					{
						UINT32 intRefresh = Math::roundToInt(refreshRate);

						if (refreshRate == devMode.dmDisplayFrequency)
						{
							foundRefreshRate = true;
							break;
						}
					}

					if (!foundRefreshRate)
					{
						win32VideoMode->mRefreshRates.push_back((float)devMode.dmDisplayFrequency);
					}

					foundVideoMode = true;
					break;
				}
			}

			if (!foundVideoMode)
			{
				Win32VideoMode* videoMode = cm_new<Win32VideoMode>(devMode.dmPelsWidth, devMode.dmPelsHeight, this);
				videoMode->mRefreshRates.push_back((float)devMode.dmDisplayFrequency);

				mVideoModes.push_back(videoMode);
			}
		}

		// Get desktop display mode
		EnumDisplaySettings(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &devMode);

		Win32VideoMode* desktopVideoMode = cm_new<Win32VideoMode>(devMode.dmPelsWidth, devMode.dmPelsHeight, this);
		desktopVideoMode->mRefreshRates.push_back((float)devMode.dmDisplayFrequency);

		mDesktopVideoMode = desktopVideoMode;
	}

	Win32VideoOutputInfo::~Win32VideoOutputInfo()
	{
		CloseHandle(mMonitorHandle);
	}

	Win32VideoMode::Win32VideoMode(UINT32 width, UINT32 height, VideoOutputInfo* outputInfo)
		:VideoMode(width, height, outputInfo)
	{ }
}
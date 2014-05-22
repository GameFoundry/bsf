#include "CmWin32VideoModeInfo.h"
#include "CmMath.h"
#include "CmException.h"

namespace BansheeEngine
{
	Win32VideoModeInfo::Win32VideoModeInfo()
	{
		DISPLAY_DEVICE displayDevice;
		memset(&displayDevice, 0, sizeof(DISPLAY_DEVICE));
		displayDevice.cb = sizeof(DISPLAY_DEVICE);

		UINT32 i = 0;
		while (EnumDisplayDevices(nullptr, i++, &displayDevice, 0))
		{
			if ((displayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) == 0)
				continue;

			mOutputs.push_back(cm_new<Win32VideoOutputInfo>(displayDevice.DeviceName));
		}
	}

	Win32VideoOutputInfo::Win32VideoOutputInfo(char* deviceName)
		:mMonitorHandle(0)
	{
		mName = deviceName;

		DISPLAY_DEVICE displayDevice;
		displayDevice.cb = sizeof(DISPLAY_DEVICE);

		UINT32 i = 0;
		while (EnumDisplayDevices(deviceName, i++, &displayDevice, EDD_GET_DEVICE_INTERFACE_NAME))
		{
			if (displayDevice.StateFlags & DISPLAY_DEVICE_ACTIVE)
			{
				mMonitorHandle = (HMONITOR)CreateFile(displayDevice.DeviceID, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
			}
		}

		DEVMODE devMode;
		devMode.dmSize = sizeof(DEVMODE);
		devMode.dmDriverExtra = 0;

		i = 0;
		while (EnumDisplaySettings(deviceName, i++, &devMode))
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
		EnumDisplaySettings(deviceName, ENUM_CURRENT_SETTINGS, &devMode);

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
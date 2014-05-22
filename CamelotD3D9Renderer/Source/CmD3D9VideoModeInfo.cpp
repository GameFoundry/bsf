#include "CmD3D9VideoModeInfo.h"
#include "CmD3D9RenderSystem.h"
#include "CmException.h"

namespace BansheeEngine
{
	D3D9VideoModeInfo::D3D9VideoModeInfo(IDirect3D9* d3d9device)
	{
		for (UINT32 i = 0; i < d3d9device->GetAdapterCount(); i++)
		{
			D3DADAPTER_IDENTIFIER9 adapterIdentifier;
			D3DCAPS9 d3dcaps9;

			d3d9device->GetAdapterIdentifier(i, 0, &adapterIdentifier);
			d3d9device->GetDeviceCaps(i, D3DDEVTYPE_HAL, &d3dcaps9);

			mOutputs.push_back(cm_new<D3D9VideoOutputInfo>(d3d9device, i));
		}
	}

	D3D9VideoOutputInfo::D3D9VideoOutputInfo(IDirect3D9* d3d9device, UINT32 adapterIdx)
		:mMonitorHandle(0)
	{
		IDirect3D9* pD3D = D3D9RenderSystem::getDirect3D9();

		D3DADAPTER_IDENTIFIER9 adapterIdentifier;
		pD3D->GetAdapterIdentifier(adapterIdx, 0, &adapterIdentifier);

		mName = adapterIdentifier.DeviceName;

		for (UINT32 i = 0; i < pD3D->GetAdapterModeCount(adapterIdx, D3DFMT_X8R8G8B8); i++)
		{
			D3DDISPLAYMODE displayMode;
			pD3D->EnumAdapterModes(adapterIdx, D3DFMT_X8R8G8B8, i, &displayMode);

			bool foundVideoMode = false;
			for (auto videoMode : mVideoModes)
			{
				D3D9VideoMode* d3d9videoMode = static_cast<D3D9VideoMode*>(videoMode);

				if (d3d9videoMode->mWidth == displayMode.Width && d3d9videoMode->mHeight == displayMode.Height)
				{
					bool foundRefreshRate = false;
					for (auto refreshRate : d3d9videoMode->mRefreshRates)
					{
						UINT32 intRefresh = Math::roundToInt(refreshRate);

						if (refreshRate == displayMode.RefreshRate)
						{
							foundRefreshRate = true;
							break;
						}
					}

					if (!foundRefreshRate)
					{
						d3d9videoMode->mRefreshRates.push_back((float)displayMode.RefreshRate);
					}

					foundVideoMode = true;
					break;
				}
			}

			if (!foundVideoMode)
			{
				D3D9VideoMode* videoMode = cm_new<D3D9VideoMode>(displayMode.Width, displayMode.Height, this);
				videoMode->mRefreshRates.push_back((float)displayMode.RefreshRate);

				mVideoModes.push_back(videoMode);
			}
		}

		// Get desktop display mode
		HMONITOR hMonitor = pD3D->GetAdapterMonitor(adapterIdx);
		MONITORINFOEX monitorInfo;
		monitorInfo.cbSize = sizeof(MONITORINFOEX);
		GetMonitorInfo(hMonitor, &monitorInfo);

		DEVMODE devMode;
		devMode.dmSize = sizeof(DEVMODE);
		devMode.dmDriverExtra = 0;
		EnumDisplaySettings(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &devMode);

		D3D9VideoMode* desktopVideoMode = cm_new<D3D9VideoMode>(devMode.dmPelsWidth, devMode.dmPelsHeight, this);
		desktopVideoMode->mRefreshRates.push_back((float)devMode.dmDisplayFrequency);

		mDesktopVideoMode = desktopVideoMode;
	}

	D3D9VideoMode::D3D9VideoMode(UINT32 width, UINT32 height, VideoOutputInfo* outputInfo)
		:VideoMode(width, height, outputInfo)
	{ }
}
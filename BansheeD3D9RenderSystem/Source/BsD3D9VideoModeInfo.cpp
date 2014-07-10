//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsD3D9VideoModeInfo.h"
#include "BsD3D9RenderSystem.h"
#include "BsException.h"

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

			mOutputs.push_back(bs_new<D3D9VideoOutputInfo>(d3d9device, i));
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

				UINT32 intRefresh = Math::roundToInt(d3d9videoMode->mRefreshRate);
				if (d3d9videoMode->mWidth == displayMode.Width && d3d9videoMode->mHeight == displayMode.Height && intRefresh == displayMode.RefreshRate)
				{
					foundVideoMode = true;
					break;
				}
			}

			if (!foundVideoMode)
			{
				D3D9VideoMode* videoMode = bs_new<D3D9VideoMode>(displayMode.Width, displayMode.Height, (float)displayMode.RefreshRate, adapterIdx);

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

		D3D9VideoMode* desktopVideoMode = bs_new<D3D9VideoMode>(devMode.dmPelsWidth, devMode.dmPelsHeight, (float)devMode.dmDisplayFrequency, adapterIdx);

		mDesktopVideoMode = desktopVideoMode;
	}

	D3D9VideoMode::D3D9VideoMode(UINT32 width, UINT32 height, float refreshRate, UINT32 outputIdx)
		:VideoMode(width, height, refreshRate, outputIdx)
	{ }
}
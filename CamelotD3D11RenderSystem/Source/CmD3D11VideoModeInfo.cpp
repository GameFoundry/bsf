#include "CmD3D11VideoModeInfo.h"
#include "CmException.h"

namespace BansheeEngine
{
	D3D11VideoModeInfo::D3D11VideoModeInfo(IDXGIAdapter* dxgiAdapter)
	{
		UINT32 outputIdx = 0;
		IDXGIOutput* output = nullptr;
		while (dxgiAdapter->EnumOutputs(outputIdx, &output) != DXGI_ERROR_NOT_FOUND)
		{
			outputIdx++;
			mOutputs.push_back(cm_new<D3D11VideoOutputInfo>(output));
		}
	}

	D3D11VideoOutputInfo::D3D11VideoOutputInfo(IDXGIOutput* output)
		:mDXGIOutput(output)
	{
		DXGI_OUTPUT_DESC outputDesc;
		output->GetDesc(&outputDesc);
		mName = toString(WString(outputDesc.DeviceName));

		UINT32 numModes = 0;

		HRESULT hr = output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &numModes, 0);
		if (FAILED(hr))
		{
			SAFE_RELEASE(output);
			CM_EXCEPT(InternalErrorException, "Error while enumerating adapter output video modes.");
		}

		DXGI_MODE_DESC* modeDesc = cm_newN<DXGI_MODE_DESC>(numModes);

		hr = output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &numModes, modeDesc);
		if (FAILED(hr))
		{
			cm_deleteN(modeDesc, numModes);

			SAFE_RELEASE(output);
			CM_EXCEPT(InternalErrorException, "Error while enumerating adapter output video modes.");
		}

		for (UINT32 i = 0; i < numModes; i++)
		{
			DXGI_MODE_DESC displayMode = modeDesc[i];

			bool foundVideoMode = false;
			for (auto videoMode : mVideoModes)
			{
				D3D11VideoMode* d3d11videoMode = static_cast<D3D11VideoMode*>(videoMode);

				if (d3d11videoMode->mWidth == displayMode.Width && d3d11videoMode->mHeight == displayMode.Height)
				{
					bool foundRefreshRate = false;
					for (auto refreshRate : d3d11videoMode->mD3D11RefreshRates)
					{
						if (refreshRate.numerator == displayMode.RefreshRate.Numerator &&
							refreshRate.denominator == displayMode.RefreshRate.Denominator)
						{
							foundRefreshRate = true;
							break;
						}
					}

					if (!foundRefreshRate)
					{
						D3D11VideoMode::RefreshRate refreshRate;
						refreshRate.numerator = displayMode.RefreshRate.Numerator;
						refreshRate.denominator = displayMode.RefreshRate.Denominator;

						d3d11videoMode->mD3D11RefreshRates.push_back(refreshRate);
						d3d11videoMode->mRefreshRates.push_back(refreshRate.numerator / (float)refreshRate.denominator);
					}

					foundVideoMode = true;
					break;
				}
			}

			if (!foundVideoMode)
			{
				D3D11VideoMode* videoMode = cm_new<D3D11VideoMode>(displayMode.Width, displayMode.Height, this, displayMode);

				D3D11VideoMode::RefreshRate refreshRate;
				refreshRate.numerator = displayMode.RefreshRate.Numerator;
				refreshRate.denominator = displayMode.RefreshRate.Denominator;

				videoMode->mD3D11RefreshRates.push_back(refreshRate);
				videoMode->mRefreshRates.push_back(refreshRate.numerator / (float)refreshRate.denominator);

				mVideoModes.push_back(videoMode);
			}
		}

		cm_deleteN(modeDesc, numModes);

		// Get desktop display mode
		HMONITOR hMonitor = outputDesc.Monitor;
		MONITORINFOEX monitorInfo;
		monitorInfo.cbSize = sizeof(MONITORINFOEX);
		GetMonitorInfo(hMonitor, &monitorInfo);

		DEVMODE devMode;
		devMode.dmSize = sizeof(DEVMODE);
		devMode.dmDriverExtra = 0;
		EnumDisplaySettings(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &devMode);

		DXGI_MODE_DESC currentMode;
		currentMode.Width = devMode.dmPelsWidth;
		currentMode.Height = devMode.dmPelsHeight;
		bool useDefaultRefreshRate = 1 == devMode.dmDisplayFrequency || 0 == devMode.dmDisplayFrequency;
		currentMode.RefreshRate.Numerator = useDefaultRefreshRate ? 0 : devMode.dmDisplayFrequency;
		currentMode.RefreshRate.Denominator = useDefaultRefreshRate ? 0 : 1;
		currentMode.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		currentMode.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		currentMode.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		DXGI_MODE_DESC nearestMode;
		ZeroMemory(&nearestMode, sizeof(nearestMode));

		output->FindClosestMatchingMode(&currentMode, &nearestMode, nullptr);

		D3D11VideoMode* desktopVideoMode = cm_new<D3D11VideoMode>(nearestMode.Width, nearestMode.Height, this, nearestMode);;
		
		{
			D3D11VideoMode::RefreshRate refreshRate;
			refreshRate.numerator = nearestMode.RefreshRate.Numerator;
			refreshRate.denominator = nearestMode.RefreshRate.Denominator;

			desktopVideoMode->mD3D11RefreshRates.push_back(refreshRate);
			desktopVideoMode->mRefreshRates.push_back(refreshRate.numerator / (float)refreshRate.denominator);
		}

		mDesktopVideoMode = desktopVideoMode;
	}

	D3D11VideoOutputInfo::~D3D11VideoOutputInfo()
	{
		SAFE_RELEASE(mDXGIOutput);
	}

	D3D11VideoMode::D3D11VideoMode(UINT32 width, UINT32 height, VideoOutputInfo* outputInfo, DXGI_MODE_DESC dxgiMode)
		:VideoMode(width, height, outputInfo), mDXGIMode(dxgiMode)
	{ }
}
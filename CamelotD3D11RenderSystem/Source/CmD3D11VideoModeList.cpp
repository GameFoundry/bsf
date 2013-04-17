#include "CmD3D11VideoModeList.h"
#include "CmException.h"
#include "CmD3D11Driver.h"

namespace CamelotFramework
{
	D3D11VideoModeList::D3D11VideoModeList(D3D11Driver* driver, UINT32 adapterOutput)
	{
		if(driver == nullptr)
			CM_EXCEPT(InvalidParametersException, "driver parameter is NULL");

		mDriver = driver;
		enumerate(adapterOutput);
	}

	D3D11VideoModeList::~D3D11VideoModeList()
	{
		mDriver = NULL;
		mModeList.clear();
	}

	void D3D11VideoModeList::enumerate(UINT32 adapterOutput)
	{
		UINT adapter = mDriver->getAdapterNumber();
		HRESULT hr;

		IDXGIOutput* output;
		hr = mDriver->getDeviceAdapter()->EnumOutputs(adapterOutput, &output);
		if(hr == DXGI_ERROR_NOT_FOUND)
			CM_EXCEPT(InvalidParametersException, "Adapter output " + toString(adapterOutput) + " not found!");

		if(FAILED(hr))
			CM_EXCEPT(InternalErrorException, "Error while enumerating adapter output video modes. Output index: " + toString(adapterOutput));

		DXGI_OUTPUT_DESC outputDesc;
		output->GetDesc(&outputDesc);

		UINT32 numModes = 0;

		hr = output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &numModes, 0);
		if(FAILED(hr))
		{
			SAFE_RELEASE(output);
			CM_EXCEPT(InternalErrorException, "Error while enumerating adapter output video modes. Output index: " + toString(adapterOutput));
		}

		DXGI_MODE_DESC* modeDesc = CM_NEW_ARRAY(DXGI_MODE_DESC, numModes, ScratchAlloc);

		hr = output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &numModes, modeDesc);
		if(FAILED(hr))
		{
			CM_DELETE_ARRAY(modeDesc, DXGI_MODE_DESC, numModes, ScratchAlloc);

			SAFE_RELEASE(output);
			CM_EXCEPT(InternalErrorException, "Error while enumerating adapter output video modes. Output index: " + toString(adapterOutput));
		}

		SAFE_RELEASE(output);

		for(UINT32 i = 0; i < numModes; i++)
		{
			DXGI_MODE_DESC displayMode = modeDesc[i];

			// Filter out low-resolutions
			if(displayMode.Width < 640 || displayMode.Height < 400)
				continue;

			// Check to see if it is already in the list (to filter out refresh rates)
			bool found = false;
			for(auto it = mModeList.begin(); it != mModeList.end(); it++)
			{
				DXGI_MODE_DESC existingMode = it->getModeDesc();
				if(	existingMode.Width == displayMode.Width && 
					existingMode.Height == displayMode.Height &&
					existingMode.Format == displayMode.Format &&
					existingMode.RefreshRate.Numerator == displayMode.RefreshRate.Numerator &&
					existingMode.RefreshRate.Denominator == displayMode.RefreshRate.Denominator
					)
				{
					found = true;
					break;
				}
			}

			if(!found)
				mModeList.push_back(D3D11VideoMode(outputDesc, displayMode));
		}

		CM_DELETE_ARRAY(modeDesc, DXGI_MODE_DESC, numModes, ScratchAlloc);
	}

	UINT32 D3D11VideoModeList::count() const
	{
		return (UINT32)mModeList.size();
	}

	const D3D11VideoMode& D3D11VideoModeList::item(UINT32 idx) const
	{
		return mModeList.at(idx);
	}

	const D3D11VideoMode& D3D11VideoModeList::item(const String &name) const
	{
		for (auto iter = mModeList.begin(); iter != mModeList.end(); ++iter)
		{
			if (iter->getDescription() == name)
				return (*iter);
		}

		CM_EXCEPT(InvalidParametersException, "Cannot find video mode with the specified name.");
	}
}
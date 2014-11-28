#include "BsD3D9DeviceManager.h"
#include "BsD3D9Device.h"
#include "BsD3D9RenderAPI.h"
#include "BsD3D9RenderWindow.h"
#include "BsD3D9Driver.h"
#include "BsD3D9DriverList.h"
#include "BsException.h"

namespace BansheeEngine
{
	D3D9DeviceManager::D3D9DeviceManager()
		:mActiveDevice(nullptr), mActiveRenderWindowDevice(nullptr)
	{

	}

	D3D9DeviceManager::~D3D9DeviceManager()
	{
		while (mRenderDevices.size() > 0)	
			mRenderDevices[0]->destroy();						

		mActiveDevice = nullptr;
		mActiveRenderWindowDevice = nullptr;
	}

	void D3D9DeviceManager::setActiveDevice(D3D9Device* device)
	{
		if (mActiveDevice != device)
		{
			mActiveDevice = device;

			D3D9RenderAPI* renderSystem = static_cast<D3D9RenderAPI*>(BansheeEngine::RenderAPICore::instancePtr());
			D3D9DriverList*	driverList = renderSystem->getDirect3DDrivers();

			// Update the active driver member.
			for (UINT32 i=0; i < driverList->count(); ++i)
			{
				D3D9Driver* currDriver = driverList->item(i);
				if (currDriver->getAdapterNumber() == mActiveDevice->getAdapterNumber())
				{
					renderSystem->mActiveD3DDriver = currDriver;
					break;
				}				
			}	
		}						
	}

	D3D9Device* D3D9DeviceManager::getActiveDevice()
	{	
		if (mActiveDevice == nullptr)
			BS_EXCEPT(InvalidParametersException, "Current active device is null." );

		return mActiveDevice;		
	}

	void D3D9DeviceManager::setActiveRenderTargetDevice(D3D9Device* device)
	{
		mActiveRenderWindowDevice = device;
		if (mActiveRenderWindowDevice != nullptr)
			setActiveDevice(mActiveRenderWindowDevice);			
	}

	D3D9Device* D3D9DeviceManager::getActiveRenderTargetDevice()
	{
		return mActiveRenderWindowDevice;
	}

	UINT D3D9DeviceManager::getDeviceCount()
	{
		return static_cast<UINT>(mRenderDevices.size());
	}

	D3D9Device* D3D9DeviceManager::getDevice(UINT index)
	{
		return mRenderDevices[index];
	}

	void D3D9DeviceManager::linkRenderWindow(D3D9RenderWindowCore* renderWindow)
	{		
		D3D9Device* renderDevice;

		// Detach from previous device.
		renderDevice = renderWindow->_getDevice();		
		if (renderDevice != nullptr)
			renderDevice->detachRenderWindow(renderWindow);						

		Vector<D3D9RenderWindowCore*> renderWindowsGroup;

		// Select new device for this window.		
		renderDevice = selectDevice(renderWindow, renderWindowsGroup);

		// Link the windows group to the new device.
		for (UINT32 i = 0; i < renderWindowsGroup.size(); ++i)
		{
			D3D9RenderWindowCore* currWindow = renderWindowsGroup[i];

			currWindow->_setDevice(renderDevice);
			renderDevice->attachRenderWindow(currWindow);
			renderDevice->setAdapterOrdinalIndex(currWindow, i);
		}
				
		renderDevice->acquire();
		if (mActiveDevice == nullptr)
			setActiveDevice(renderDevice);		
	}

	D3D9Device* D3D9DeviceManager::selectDevice(D3D9RenderWindowCore* renderWindow, Vector<D3D9RenderWindowCore*>& renderWindowsGroup)
	{
		D3D9RenderAPI* renderSystem = static_cast<D3D9RenderAPI*>(BansheeEngine::RenderAPICore::instancePtr());
		D3D9Device*	renderDevice = nullptr;	
		IDirect3D9*	direct3D9 = D3D9RenderAPI::getDirect3D9();
		UINT adapterOrdinal = D3DADAPTER_DEFAULT;
		D3DDEVTYPE devType = D3DDEVTYPE_HAL;						
		DWORD extraFlags = 0;					
		D3D9DriverList* driverList = renderSystem->getDirect3DDrivers();

		// Default group includes at least the given render window.
		renderWindowsGroup.push_back(renderWindow);

		// Try to find a matching device from current device list.
		if (renderDevice == nullptr)
		{
			for (UINT32 i = 0; i < mRenderDevices.size(); ++i)
			{
				D3D9Device* currDevice = mRenderDevices[i];

				if (currDevice->getAdapterNumber() == adapterOrdinal &&
					currDevice->getDeviceType() == devType)
				{
					renderDevice = currDevice;
					break;
				}			
			}
		}

		// No matching device found -> try reference device type (might have been 
		// previously created as a fallback, but don't change devType because HAL
		// should be preferred on creation)
		if (renderDevice == nullptr)
		{
			for (UINT32 i = 0; i < mRenderDevices.size(); ++i)
			{
				D3D9Device* currDevice = mRenderDevices[i];

				if (currDevice->getAdapterNumber() == adapterOrdinal &&
					currDevice->getDeviceType() == D3DDEVTYPE_REF)
				{
					renderDevice = currDevice;
					break;
				}			
			}
		}


		// No matching device found -> create new one.
		if (renderDevice == nullptr)
		{
			renderDevice = bs_new<D3D9Device>(this, adapterOrdinal, direct3D9->GetAdapterMonitor(adapterOrdinal), devType, extraFlags);
			mRenderDevices.push_back(renderDevice);

			if (mActiveDevice == nullptr)
				setActiveDevice(renderDevice);											
		}				

		return renderDevice;	
	}

	D3D9Driver* D3D9DeviceManager::findDriver(D3D9RenderWindowCore* renderWindow)
	{
		D3D9RenderAPI* renderSystem = static_cast<D3D9RenderAPI*>(BansheeEngine::RenderAPICore::instancePtr());		
		IDirect3D9*	direct3D9 = D3D9RenderAPI::getDirect3D9();				
		HMONITOR renderWindowMonitor = NULL;			
		D3D9DriverList* driverList = renderSystem->getDirect3DDrivers();

		// Find the monitor this render window belongs to.
		renderWindowMonitor = MonitorFromWindow(renderWindow->_getWindowHandle(), MONITOR_DEFAULTTONEAREST);

		// Find the matching driver using window monitor handle.
		for (UINT32 i = 0; i < driverList->count(); ++i)
		{
			D3D9Driver* currDriver = driverList->item(i);
			HMONITOR hCurrAdpaterMonitor = direct3D9->GetAdapterMonitor(currDriver->getAdapterNumber());

			if (hCurrAdpaterMonitor == renderWindowMonitor)
			{
				return currDriver;				
			}
		}

		return nullptr;
	}

	void D3D9DeviceManager::notifyOnDeviceDestroy(D3D9Device* device)
	{
		if (device != nullptr)
		{						
			if (device == mActiveDevice)			
				mActiveDevice = nullptr;

			auto iter = mRenderDevices.begin();
			while (iter != mRenderDevices.end())
			{			
				if (*iter == device)
				{					
					if(device != nullptr)
						bs_delete(device);

					mRenderDevices.erase(iter);
					break;
				}												
				++iter;
			}

			if (mActiveDevice == nullptr)
			{
				auto iter = mRenderDevices.begin();
				if (iter != mRenderDevices.end())				
					mActiveDevice = (*iter);
			}
		}
	}

	D3D9Device*	D3D9DeviceManager::getDeviceFromD3D9Device(IDirect3DDevice9* d3d9Device)
	{
		for (auto& device : mRenderDevices)
		{
			if (device->getD3D9Device() == d3d9Device)
				return device;
		}

		return nullptr;
	}
}

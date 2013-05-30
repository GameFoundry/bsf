/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#include "CmD3D9DeviceManager.h"
#include "CmD3D9Device.h"
#include "CmD3D9RenderSystem.h"
#include "CmD3D9RenderWindow.h"
#include "CmD3D9Driver.h"
#include "CmD3D9DriverList.h"
#include "CmException.h"

namespace CamelotFramework
{
	//---------------------------------------------------------------------
	D3D9DeviceManager::D3D9DeviceManager()
	{
		mActiveDevice = NULL;		
		mActiveRenderWindowDevice = NULL;
	}

	//---------------------------------------------------------------------
	D3D9DeviceManager::~D3D9DeviceManager()
	{
		DeviceIterator itDevice = mRenderDevices.begin();
		while (mRenderDevices.size() > 0)
		{			
			mRenderDevices[0]->destroy();						
		}		

		mActiveDevice = NULL;
		mActiveRenderWindowDevice = NULL;
	}

	//---------------------------------------------------------------------
	void D3D9DeviceManager::setActiveDevice(D3D9Device* device)
	{
		if (mActiveDevice != device)
		{
			mActiveDevice = device;

			D3D9RenderSystem* renderSystem = static_cast<D3D9RenderSystem*>(CamelotFramework::RenderSystem::instancePtr());
			D3D9DriverList*		driverList	 = renderSystem->getDirect3DDrivers();

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

	//---------------------------------------------------------------------
	D3D9Device* D3D9DeviceManager::getActiveDevice()
	{	
		if (mActiveDevice == NULL)
		{
			CM_EXCEPT(InvalidParametersException, "Current active device is NULL !!!" );
		}

		return mActiveDevice;		
	}

	//---------------------------------------------------------------------
	void D3D9DeviceManager::setActiveRenderTargetDevice(D3D9Device* device)
	{
		mActiveRenderWindowDevice = device;
		if (mActiveRenderWindowDevice != NULL)		
			setActiveDevice(mActiveRenderWindowDevice);			
	}

	//---------------------------------------------------------------------
	D3D9Device* D3D9DeviceManager::getActiveRenderTargetDevice()
	{
		return mActiveRenderWindowDevice;
	}

	//---------------------------------------------------------------------
	UINT D3D9DeviceManager::getDeviceCount()
	{
		return static_cast<UINT>(mRenderDevices.size());
	}

	//---------------------------------------------------------------------
	D3D9Device* D3D9DeviceManager::getDevice(UINT index)
	{
		return mRenderDevices[index];
	}

	//---------------------------------------------------------------------
	void D3D9DeviceManager::linkRenderWindow(D3D9RenderWindow* renderWindow)
	{		
		D3D9Device* renderDevice;

		// Detach from previous device.
		renderDevice = renderWindow->getDevice();		
		if (renderDevice != NULL)		
			renderDevice->detachRenderWindow(renderWindow);						

		D3D9RenderWindowList renderWindowsGroup;

		// Select new device for this window.		
		renderDevice = selectDevice(renderWindow, renderWindowsGroup);

		// Link the windows group to the new device.
		for (UINT32 i = 0; i < renderWindowsGroup.size(); ++i)
		{
			D3D9RenderWindow* currWindow = renderWindowsGroup[i];

			currWindow->setDevice(renderDevice);
			renderDevice->attachRenderWindow(currWindow);
			renderDevice->setAdapterOrdinalIndex(currWindow, i);
		}
				
		renderDevice->acquire();
		if (mActiveDevice == NULL)			
			setActiveDevice(renderDevice);		
	}

	//---------------------------------------------------------------------
	D3D9Device* D3D9DeviceManager::selectDevice(D3D9RenderWindow* renderWindow, D3D9RenderWindowList& renderWindowsGroup)
	{
		D3D9RenderSystem*		renderSystem	 = static_cast<D3D9RenderSystem*>(CamelotFramework::RenderSystem::instancePtr());
		D3D9Device*				renderDevice	 = NULL;	
		IDirect3D9*				direct3D9	     = D3D9RenderSystem::getDirect3D9();
		UINT					nAdapterOrdinal  = D3DADAPTER_DEFAULT;
		D3DDEVTYPE				devType			 = D3DDEVTYPE_HAL;						
		DWORD					extraFlags		 = 0;					
		D3D9DriverList*			driverList = renderSystem->getDirect3DDrivers();
		bool					nvAdapterFound = false;


		// Default group includes at least the given render window.
		renderWindowsGroup.push_back(renderWindow);

		// Case we use nvidia performance HUD, override the device settings. 
		if (renderWindow->isNvPerfHUDEnable())
		{
			// Look for 'NVIDIA NVPerfHUD' adapter (<= v4)
			// or 'NVIDIA PerfHUD' (v5)
			// If it is present, override default settings
			for (UINT adapter=0; adapter < direct3D9->GetAdapterCount(); ++adapter)
			{
				D3D9Driver* currDriver = driverList->item(adapter);
				const D3DADAPTER_IDENTIFIER9& currAdapterIdentifier = currDriver->getAdapterIdentifier();

				if(strstr(currAdapterIdentifier.Description, "PerfHUD") != NULL)
				{
					renderDevice = NULL;
					nAdapterOrdinal = adapter;
					renderSystem->mActiveD3DDriver = currDriver;
					devType = D3DDEVTYPE_REF;
					nvAdapterFound = true;
					break;
				}
			}		
		}

		// Try to find a matching device from current device list.
		if (renderDevice == NULL)
		{
			for (UINT32 i = 0; i < mRenderDevices.size(); ++i)
			{
				D3D9Device* currDevice = mRenderDevices[i];

				if (currDevice->getAdapterNumber() == nAdapterOrdinal &&
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
		if (renderDevice == NULL)
		{
			for (UINT32 i = 0; i < mRenderDevices.size(); ++i)
			{
				D3D9Device* currDevice = mRenderDevices[i];

				if (currDevice->getAdapterNumber() == nAdapterOrdinal &&
					currDevice->getDeviceType() == D3DDEVTYPE_REF)
				{
					renderDevice = currDevice;
					break;
				}			
			}
		}


		// No matching device found -> create new one.
		if (renderDevice == NULL)
		{
			renderDevice = cm_new<D3D9Device>(this, nAdapterOrdinal, direct3D9->GetAdapterMonitor(nAdapterOrdinal), devType, extraFlags);
			mRenderDevices.push_back(renderDevice);
			if (mActiveDevice == NULL)			
				setActiveDevice(renderDevice);											
		}				

		return renderDevice;	
	}

	//-----------------------------------------------------------------------
	D3D9Driver* D3D9DeviceManager::findDriver(D3D9RenderWindow* renderWindow)
	{
		D3D9RenderSystem*		renderSystem	 = static_cast<D3D9RenderSystem*>(CamelotFramework::RenderSystem::instancePtr());		
		IDirect3D9*				direct3D9	     = D3D9RenderSystem::getDirect3D9();
		UINT					nAdapterOrdinal  = D3DADAPTER_DEFAULT;						
		HMONITOR				hRenderWindowMonitor = NULL;			
		D3D9DriverList*			driverList = renderSystem->getDirect3DDrivers();

		// Find the monitor this render window belongs to.
		hRenderWindowMonitor = MonitorFromWindow(renderWindow->getWindowHandle(), MONITOR_DEFAULTTONEAREST);


		// Find the matching driver using window monitor handle.
		for (UINT32 i = 0; i < driverList->count(); ++i)
		{
			D3D9Driver* currDriver       = driverList->item(i);
			HMONITOR hCurrAdpaterMonitor = direct3D9->GetAdapterMonitor(currDriver->getAdapterNumber());

			if (hCurrAdpaterMonitor == hRenderWindowMonitor)
			{
				return currDriver;				
			}
		}

		return nullptr;
	}

	//-----------------------------------------------------------------------
	void D3D9DeviceManager::notifyOnDeviceDestroy(D3D9Device* device)
	{
		if (device != NULL)
		{						
			if (device == mActiveDevice)			
				mActiveDevice = NULL;			

			DeviceIterator itDevice = mRenderDevices.begin();
			while (itDevice != mRenderDevices.end())
			{			
				if (*itDevice == device)
				{					
					if(device != nullptr)
						cm_delete(device);

					mRenderDevices.erase(itDevice);
					break;
				}												
				++itDevice;
			}

			if (mActiveDevice == NULL)
			{
				DeviceIterator itDevice = mRenderDevices.begin();
				if (itDevice != mRenderDevices.end())				
					mActiveDevice = (*itDevice);
			}
		}
	}

	//---------------------------------------------------------------------
	D3D9Device*	D3D9DeviceManager::getDeviceFromD3D9Device(IDirect3DDevice9* d3d9Device)
	{
		DeviceIterator itDevice = mRenderDevices.begin();
		while (itDevice != mRenderDevices.end())
		{			
			if ((*itDevice)->getD3D9Device() == d3d9Device)
			{					
				return *itDevice;
			}												
			++itDevice;
		}

		return NULL;
	}

	//---------------------------------------------------------------------
	void D3D9DeviceManager::destroyInactiveRenderDevices()
	{
		// TODO PORT - This is supposed to destroy inactive render devices but lastPresentFrame isn't being set properly at the moment, so this wont work properly
		//DeviceIterator itDevice = mRenderDevices.begin();
		//while (itDevice != mRenderDevices.end())
		//{			
		//	if ((*itDevice)->getRenderWindowCount() == 0 &&
		//		(*itDevice)->getLastPresentFrame() + 1 < Root::getSingleton().getNextFrameNumber())
		//	{		
		//		if (*itDevice == mActiveRenderWindowDevice)
		//			setActiveRenderTargetDevice(NULL);
		//		(*itDevice)->destroy();
		//		break;
		//	}												
		//	++itDevice;
		//}
	}

}

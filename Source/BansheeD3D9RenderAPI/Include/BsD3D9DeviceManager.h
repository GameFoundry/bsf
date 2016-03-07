//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsD3D9Prerequisites.h"

namespace BansheeEngine 
{
	/** @addtogroup D3D9
	 *  @{
	 */

	/** Handles creation of DirectX 9 devices. Also links the devices with created render targets. */
	class BS_D3D9_EXPORT D3D9DeviceManager
	{	
	public:
		D3D9DeviceManager();
		~D3D9DeviceManager();

		/**	Changes the active device to the provided device. Must be not null. */
		void setActiveDevice(D3D9Device* device);

		/**	Retrieves the currently active device. */
		D3D9Device*	getActiveDevice();

		/**	Sets the device used by the currently active render target. */
		void setActiveRenderTargetDevice(D3D9Device* device);

		/**	Retrieves the device used by the currently active render target. */
		D3D9Device*	getActiveRenderTargetDevice();		

		/**	Returns the total number of devices available. */
		UINT getDeviceCount();

		/**	Returns the device under the specified index. */
		D3D9Device*	getDevice(UINT index);

		/**
		 * Links the provided render window with a device. The window will be assigned an existing device if a match one
		 * can be found, otherwise a new device will be created.
		 */
		void linkRenderWindow(D3D9RenderWindowCore* renderWindow);

		/**	Called by the devices when they're are being destroyed. */
		void notifyOnDeviceDestroy(D3D9Device* device);

		/**	Retrieves engine device from DX9 device. */
		D3D9Device*	getDeviceFromD3D9Device(IDirect3DDevice9* d3d9Device);

	protected:
		/**
		 * Attempts to find a matching device for the provided render window. If one cannot be found a new device is
		 * created. Found/created device is returned, as well as a list of all render windows using that device.
		 */
		D3D9Device*	selectDevice(D3D9RenderWindowCore* renderWindow, Vector<D3D9RenderWindowCore*>& renderWindowsGroup);

		/**	Finds the driver the render window belongs to. */
		D3D9Driver*	findDriver(D3D9RenderWindowCore* renderWindow);

		Vector<D3D9Device*> mRenderDevices;
		D3D9Device*	mActiveDevice;
		D3D9Device*	mActiveRenderWindowDevice;		
	};

	/** @} */
}
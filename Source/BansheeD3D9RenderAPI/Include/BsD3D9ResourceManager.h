//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsD3D9Prerequisites.h"

namespace BansheeEngine 
{
	/** @addtogroup D3D9
	 *  @{
	 */

	/**	Types of resource creation policies. */
	enum D3D9ResourceCreationPolicy
	{
		/**
		 *	Creates resources only on active device. Meaning those resources will be unavailable
		 *	for other devices.
		 */
		RCP_CREATE_ON_ACTIVE_DEVICE,

		/**
		 *	Creates resources on all devices making them available to all of them.
		 */
		RCP_CREATE_ON_ALL_DEVICES
	};
	
	/** Handles DirectX 9 resource life, notifying them about relevant device changes. */
	class BS_D3D9_EXPORT D3D9ResourceManager
	{
	public:
		D3D9ResourceManager();
		~D3D9ResourceManager();

		/**	Called after the device has been created. */
		void notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device);

		/**	Called before the device is destroyed. */
		void notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device);

		/**	Called after the device has entered the lost state. */
		void notifyOnDeviceLost(IDirect3DDevice9* d3d9Device);

		/**	Called after the device has been reset. */
		void notifyOnDeviceReset(IDirect3DDevice9* d3d9Device);

		/**	Called when device state is changing. Access to any device should be locked. */
		void lockDeviceAccess();

		/**	Called when device state change completed. Access to any device is allowed. */
		void unlockDeviceAccess();
		
		/**	Called when new resource is created. */
		void _notifyResourceCreated(D3D9Resource* pResource);

		/**	Called before resource is destroyed. */
		void _notifyResourceDestroyed(D3D9Resource* pResource);

		/**	Sets creation policy that specifies on what devices will resources be created on. */
		void setCreationPolicy(D3D9ResourceCreationPolicy creationPolicy);

		/**	Returns creation policy that specifies on what devices will resources be created on. */
		D3D9ResourceCreationPolicy getCreationPolicy() const;
		
	protected:
		friend class D3D9Resource;
		
		Mutex mResourcesMutex;
		Vector<D3D9Resource*> mResources;
		D3D9ResourceCreationPolicy mResourceCreationPolicy;
		long mDeviceAccessLockCount;		
	};

	/** @} */
}
//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsD3D9Prerequisites.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Types of resource creation policies.
	 */
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
	
	/**
	 * @brief	Handles DirectX 9 resource life, notifying them
	 *			about relevant device changes.
	 */
	class BS_D3D9_EXPORT D3D9ResourceManager
	{
	public:
		D3D9ResourceManager();
		~D3D9ResourceManager();

		/**
		 * @brief	Called after the device has been created.
		 */
		void notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device);

		/**
		 * @brief	Called before the device is destroyed.
		 */
		void notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device);

		/**
		 * @brief	Called after the device has entered the lost state.
		 */
		void notifyOnDeviceLost(IDirect3DDevice9* d3d9Device);

		/**
		 * @brief	Called after the device has been reset.
		 */
		void notifyOnDeviceReset(IDirect3DDevice9* d3d9Device);

		/**
		 * @brief	Called when device state is changing. Access to any device should be locked.
		 */
		void lockDeviceAccess();

		/**
		 * @brief	Called when device state change completed. Access to any device is allowed.
		 */
		void unlockDeviceAccess();
		
		/**
		 * @brief	Called when new resource is created.
		 */
		void _notifyResourceCreated(D3D9Resource* pResource);

		/**
		 * @brief	Called before resource is destroyed.
		 */
		void _notifyResourceDestroyed(D3D9Resource* pResource);

		/**
		 * @brief	Sets creation policy that specifies on what devices will resources be created on.
		 */
		void setCreationPolicy(D3D9ResourceCreationPolicy creationPolicy);

		/**
		 * @brief	Returns creation policy that specifies on what devices will resources be created on.
		 */
		D3D9ResourceCreationPolicy getCreationPolicy() const;
		
	protected:
		friend class D3D9Resource;
		
		BS_MUTEX(mResourcesMutex)
		Vector<D3D9Resource*> mResources;
		D3D9ResourceCreationPolicy mResourceCreationPolicy;
		long mDeviceAccessLockCount;		
	};
}
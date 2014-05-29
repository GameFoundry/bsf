#pragma once

#include "BsD3D9Prerequisites.h"

namespace BansheeEngine 
{
	enum D3D9ResourceCreationPolicy
	{
		// Creates the rendering resource on the current active device that needs it.
		// This policy should be used when video memory consumption should be minimized but
		// it might cause some performance issues when using loader resource thread since
		// a resource that was not created on specified device will be created on demand during
		// the rendering process and might cause the FPS to drop down. 
		RCP_CREATE_ON_ACTIVE_DEVICE,

		// Create the rendering resource on every existing device. 		
		// This policy should be used when working intensively with a background loader thread.
		// In that case when multiple devices exist, the resource will be created on each device
		// and will be ready to use in the core thread. 
		// The draw back can be some video memory waste in case that each device render different
		// scene and doesn't really need all the resources.
		RCP_CREATE_ON_ALL_DEVICES
	};
	
	class BS_D3D9_EXPORT D3D9ResourceManager
	{
	public:
		D3D9ResourceManager();
		~D3D9ResourceManager();

		// Called immediately after the Direct3D device has been created.
		void notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device);

		// Called before the Direct3D device is going to be destroyed.
		void notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device);

		// Called immediately after the Direct3D device has entered a lost state.
		void notifyOnDeviceLost(IDirect3DDevice9* d3d9Device);

		// Called immediately after the Direct3D device has been reset.
		void notifyOnDeviceReset(IDirect3DDevice9* d3d9Device);

		// Called when device state is changing. Access to any device should be locked.
		// Relevant for multi thread application.
		void lockDeviceAccess();

		// Called when device state change completed. Access to any device is allowed.
		// Relevant for multi thread application.
		void unlockDeviceAccess();
		
		// Called when new resource created.
		void _notifyResourceCreated(D3D9Resource* pResource);

		// Called when resource is about to be destroyed.
		void _notifyResourceDestroyed(D3D9Resource* pResource);

		void setCreationPolicy(D3D9ResourceCreationPolicy creationPolicy);
		D3D9ResourceCreationPolicy getCreationPolicy() const;
		
	protected:
		friend class D3D9Resource;
		
		BS_MUTEX(mResourcesMutex)
		Vector<D3D9Resource*> mResources;
		D3D9ResourceCreationPolicy mResourceCreationPolicy;
		long mDeviceAccessLockCount;		
	};
}
//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsD3D9Resource.h"
#include "BsD3D9ResourceManager.h"
#include "BsD3D9PixelBuffer.h"

namespace BansheeEngine
{
	D3D9ResourceManager::D3D9ResourceManager()
	{
		mResourceCreationPolicy = RCP_CREATE_ON_ALL_DEVICES;
		mDeviceAccessLockCount = 0;
	}

	D3D9ResourceManager::~D3D9ResourceManager()
	{
	
	}
 
	void D3D9ResourceManager::setCreationPolicy(D3D9ResourceCreationPolicy creationPolicy)
	{
		mResourceCreationPolicy = creationPolicy;
	}

	D3D9ResourceCreationPolicy D3D9ResourceManager::getCreationPolicy() const
	{
		return mResourceCreationPolicy;
	}

	void D3D9ResourceManager::notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device)
	{				
		Lock lock(mResourcesMutex);

		for (auto& resource : mResources)
			resource->notifyOnDeviceCreate(d3d9Device);			
	}

	void D3D9ResourceManager::notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device)
	{
		Lock lock(mResourcesMutex);

		for (auto& resource : mResources)
			resource->notifyOnDeviceDestroy(d3d9Device);
	}

	void D3D9ResourceManager::notifyOnDeviceLost(IDirect3DDevice9* d3d9Device)
	{
		Lock lock(mResourcesMutex);

		for (auto& resource : mResources)
			resource->notifyOnDeviceLost(d3d9Device);
	}

	void D3D9ResourceManager::notifyOnDeviceReset(IDirect3DDevice9* d3d9Device)
	{		
		Lock lock(mResourcesMutex);

		for (auto& resource : mResources)
			resource->notifyOnDeviceReset(d3d9Device);	
	}

	void D3D9ResourceManager::_notifyResourceCreated(D3D9Resource* pResource)
	{		
		Lock lock(mResourcesMutex);
		mResources.push_back(pResource);
	}
	
	void D3D9ResourceManager::_notifyResourceDestroyed(D3D9Resource* pResource)
	{		
		Lock lock(mResourcesMutex);

		auto iter = mResources.begin();
		while (iter != mResources.end())
		{
			if ((*iter) == pResource)
			{
				mResources.erase(iter);
				break;
			}			
			++iter;
		}	
	}
	
	void D3D9ResourceManager::lockDeviceAccess()
	{	
		assert(mDeviceAccessLockCount >= 0);
		mDeviceAccessLockCount++;
		if (mDeviceAccessLockCount == 1)
		{						
			D3D9Resource::lockDeviceAccess();
			D3D9PixelBuffer::lockDeviceAccess();
		}
	}

	void D3D9ResourceManager::unlockDeviceAccess()
	{
		assert(mDeviceAccessLockCount > 0);		
		mDeviceAccessLockCount--;				
		if (mDeviceAccessLockCount == 0)
		{						
			D3D9PixelBuffer::unlockDeviceAccess();
			D3D9Resource::unlockDeviceAccess();				
		}
	}
}

//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsD3D9Resource.h"
#include "BsD3D9ResourceManager.h"
#include "BsD3D9RenderAPI.h"

namespace BansheeEngine
{
	Mutex D3D9Resource::msDeviceAccessMutex;

	D3D9Resource::D3D9Resource()
	{				
		D3D9RenderAPI::getResourceManager()->_notifyResourceCreated(static_cast<D3D9Resource*>(this));		
	}

	D3D9Resource::~D3D9Resource()
	{		
		D3D9RenderAPI::getResourceManager()->_notifyResourceDestroyed(static_cast<D3D9Resource*>(this));	
	}
	
	void D3D9Resource::lockDeviceAccess()
	{		
		D3D9_DEVICE_ACCESS_LOCK;								
	}
	
	void D3D9Resource::unlockDeviceAccess()
	{		
		D3D9_DEVICE_ACCESS_UNLOCK;				
	}
}

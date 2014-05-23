#include "CmD3D9Resource.h"
#include "CmD3D9ResourceManager.h"
#include "CmD3D9RenderSystem.h"

namespace BansheeEngine
{
	CM_STATIC_MUTEX_INSTANCE(D3D9Resource::msDeviceAccessMutex)

	D3D9Resource::D3D9Resource()
	{				
		D3D9RenderSystem::getResourceManager()->_notifyResourceCreated(static_cast<D3D9Resource*>(this));		
	}

	D3D9Resource::~D3D9Resource()
	{		
		D3D9RenderSystem::getResourceManager()->_notifyResourceDestroyed(static_cast<D3D9Resource*>(this));	
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

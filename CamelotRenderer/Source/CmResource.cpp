#include "CmResource.h"
#include "CmResourceRTTI.h"

namespace CamelotEngine
{
	//void Resource::unload() 
	//{ 
	//	// Early-out without lock (mitigate perf cost of ensuring unloaded)
	//	LoadingState old = mLoadingState.get();
	//	if (old!=LOADSTATE_LOADED && old!=LOADSTATE_PREPARED) return;


	//	if (!mLoadingState.cas(old,LOADSTATE_UNLOADING)) return;

	//	// Scope lock for actual unload
	//	{
	//		OGRE_LOCK_AUTO_MUTEX
	//			if (old==LOADSTATE_PREPARED) {
	//				unprepareImpl();
	//			} else {
	//				preUnloadImpl();
	//				unloadImpl();
	//				postUnloadImpl();
	//			}
	//	}

	//	mLoadingState.set(LOADSTATE_UNLOADED);

	//	// Notify manager
	//	// Note if we have gone from PREPARED to UNLOADED, then we haven't actually
	//	// unloaded, i.e. there is no memory freed on the GPU.
	//	if(old==LOADSTATE_LOADED && mCreator)
	//		mCreator->_notifyResourceUnloaded(this);

	//	_fireUnloadingComplete();


	//}
	////-----------------------------------------------------------------------
	//void Resource::reload() 
	//{ 
	//	OGRE_LOCK_AUTO_MUTEX
	//		if (mLoadingState.get() == LOADSTATE_LOADED)
	//		{
	//			unload();
	//			load();
	//		}
	//}
		
	RTTITypeBase* Resource::getRTTIStatic()
	{
		return ResourceRTTI::instance();
	}

	RTTITypeBase* Resource::getRTTI() const
	{
		return Resource::getRTTIStatic();
	}
}
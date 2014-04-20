#include "CmCoreObjectManager.h"
#include "CmCoreObject.h"
#include "CmException.h"

namespace BansheeEngine
{
	CoreGpuObjectManager::CoreGpuObjectManager()
		:mNextAvailableID(1)
	{

	}

	CoreGpuObjectManager::~CoreGpuObjectManager()
	{
		CM_LOCK_MUTEX(mObjectsMutex);

		if(mObjects.size() > 0)
		{
			// All objects MUST be destroyed at this point, otherwise there might be memory corruption.
			// (Reason: This is called on application shutdown and at that point we also unload any dynamic libraries, 
			// which will invalidate any pointers to objects created from those libraries. Therefore we require of the user to 
			// clean up all objects manually before shutting down the application).
			CM_EXCEPT(InternalErrorException, "Core GPU object manager destroyed, but not all objects were released. User must release ALL " \
				"engine objects before application shutdown.");
		}
	}

	UINT64 CoreGpuObjectManager::registerObject(CoreObject* object)
	{
		assert(object != nullptr);

		CM_LOCK_MUTEX(mObjectsMutex);

		mObjects[mNextAvailableID] = object;

		return mNextAvailableID++;
	}

	void CoreGpuObjectManager::unregisterObject(CoreObject* object)
	{
		assert(object != nullptr);

		CM_LOCK_MUTEX(mObjectsMutex);

		mObjects.erase(object->getInternalID());
	}
}
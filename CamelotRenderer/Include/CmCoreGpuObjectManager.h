#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"

namespace CamelotEngine
{
	// TODO - Add debug option that would remember a call stack for each resource initialization,
	// so when we fail to release one we know which one it is.
	class CM_EXPORT CoreGpuObjectManager : public Module<CoreGpuObjectManager>
	{
	public:
		CoreGpuObjectManager();
		~CoreGpuObjectManager();

		UINT64 registerObject(CoreGpuObject* object);
		void unregisterObject(CoreGpuObject* object);

		void registerObjectToDestroy(std::shared_ptr<CoreGpuObject> object);
		void unregisterObjectToDestroy(std::shared_ptr<CoreGpuObject> object);
	private:
		// Keeps a list of ALL loaded core GPU objects
		UINT64 mNextAvailableID;
		map<UINT64, CoreGpuObject*>::type mObjects;
		CM_MUTEX(mObjectsMutex);

		// Keeps a list of GPU objects scheduled to be destroyed. We need a reference
		// to the object here, otherwise the object may reach reference count of 0 and be
		// deleted before its scheduled destroy() method is called. Object is added to the list
		// when destroy() is initialized, and removed when it is finished.
		map<UINT64, std::shared_ptr<CoreGpuObject>>::type mObjectsToDestroy;
		CM_MUTEX(mObjectsToDestroyMutex);
	};
}
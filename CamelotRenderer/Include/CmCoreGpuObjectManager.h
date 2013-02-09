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
	private:
		// Keeps a list of ALL loaded core GPU objects
		UINT64 mNextAvailableID;
		map<UINT64, CoreGpuObject*>::type mObjects;
		CM_MUTEX(mObjectsMutex);
	};
}